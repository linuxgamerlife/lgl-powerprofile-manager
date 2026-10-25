#include "ui/window.h"

#include "core/profile_data.h"
#include "core/theme.h"
#include "core/tuned.h"
#include "core/validation.h"
#include "ui/helpwindow.h"

#include <gdk/gdkkeysyms.h>
#include <glib.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

namespace ppp {
namespace {

Gtk::Box& make_card(const char* title, const bool flush = false) {
    auto* card = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    card->add_css_class("pp-card");
    if (flush) {
        card->add_css_class("flush");
        card->set_overflow(Gtk::Overflow::HIDDEN);
    }
    auto* head = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    head->add_css_class("card-head");
    auto* heading = Gtk::make_managed<Gtk::Label>(title);
    heading->set_halign(Gtk::Align::START);
    heading->add_css_class("section-label");
    head->append(*heading);
    card->append(*head);
    return *card;
}

void add_info_row(Gtk::Grid& grid, const int row, const char* label, Gtk::Label& value) {
    auto* key = Gtk::make_managed<Gtk::Label>(label);
    key->set_halign(Gtk::Align::START);
    key->add_css_class("muted");
    value.set_halign(Gtk::Align::START);
    value.set_hexpand(true);
    value.set_selectable(true);
    value.add_css_class("mono");
    grid.attach(*key, 0, row);
    grid.attach(value, 1, row);
}

Gtk::Label& text_label(const std::string& text) {
    auto* label = Gtk::make_managed<Gtk::Label>(text);
    label->set_halign(Gtk::Align::START);
    label->set_xalign(0.0F);
    label->set_wrap(true);
    return *label;
}

Gtk::Label& code_label(const std::string& text) {
    auto* label = Gtk::make_managed<Gtk::Label>(text);
    label->set_halign(Gtk::Align::START);
    label->set_xalign(0.0F);
    label->set_selectable(true);
    label->add_css_class("mono");
    label->add_css_class("code-block");
    return *label;
}

Gtk::Box& detail_block(const char* heading, const std::string& text) {
    auto* block = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    block->set_hexpand(true);
    auto* title = Gtk::make_managed<Gtk::Label>(heading);
    title->set_halign(Gtk::Align::START);
    title->add_css_class("reference-label");
    block->append(*title);
    block->append(text_label(text));
    return *block;
}

Gtk::Box& profile_row_content(const ProfileInfo& info) {
    auto* content = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    auto* head = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto* name = Gtk::make_managed<Gtk::Label>(info.name);
    name->add_css_class("mono");
    name->add_css_class("profile-name");
    head->append(*name);
    if (!info.label.empty()) {
        auto* label = Gtk::make_managed<Gtk::Label>(info.label);
        label->add_css_class("muted");
        label->add_css_class("small");
        head->append(*label);
    }
    content->append(*head);

    const std::string description = info.description.empty()
                                        ? "Custom tuned profile installed on this system."
                                        : info.description;
    content->append(text_label(description));
    return *content;
}

Gtk::Box& reference_item(const ProfileInfo& entry) {
    auto* item = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    item->add_css_class("reference-item");

    auto* head = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto* name = Gtk::make_managed<Gtk::Label>(entry.name);
    name->add_css_class("mono");
    name->add_css_class("profile-name");
    head->append(*name);
    auto* label = Gtk::make_managed<Gtk::Label>(entry.label);
    label->add_css_class("profile-label");
    head->append(*label);
    item->append(*head);

    item->append(text_label(entry.description));

    auto* use_cases = Gtk::make_managed<Gtk::Grid>();
    use_cases->set_column_spacing(12);
    use_cases->set_column_homogeneous(true);
    use_cases->attach(detail_block("BEST FOR", entry.best_for), 0, 0);
    use_cases->attach(detail_block("AVOID WHEN", entry.avoid_when), 1, 0);
    item->append(*use_cases);
    return *item;
}

std::string timestamp() {
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local{};
    localtime_r(&now, &local);
    std::ostringstream stream;
    stream << std::put_time(&local, "%H:%M:%S");
    return stream.str();
}

} // namespace

Window::~Window() = default;

Window::Window(const Glib::RefPtr<Gtk::Application>& application, const bool start_minimized)
    : application_(application), start_minimized_(start_minimized) {
    set_application(application_);
    set_title("LGL Power Profile Manager");
    set_default_size(820, 660);
    set_size_request(640, 520);

    build_header();
    build_pages();
    install_actions();
    set_child(content_);
    signal_hide().connect(sigc::mem_fun(*this, &Window::on_window_hidden));
    apply_title_bar(settings_.system_title_bar(), false);
    bind_header_actions();
    theme_watcher_ = std::make_unique<ThemeWatcher>(*this);
    theme_watcher_->signal_changed().connect([this] {
        if (help_window_) {
            help_window_->set_mono_family(theme_watcher_->mono_family());
        }
    });

    if (!tool_exists("tuned-adm")) {
        enter_setup_mode();
    } else {
        ready_ = true;
        set_busy(false);
        refresh_profiles();
        refresh_status();
        refresh_timer_ = Glib::signal_timeout().connect_seconds(
            [this] {
                refresh_status_quietly();
                return true;
            },
            5);
    }

    if (const char* snapshot_page = g_getenv("PPP_GTK_SNAPSHOT_PAGE"); snapshot_page != nullptr) {
        const std::string page(snapshot_page);
        const std::vector<std::string> names{"Status", "Profiles", "Log", "Reference", "Setup"};
        const auto match = std::find(names.begin(), names.end(), page);
        if (match != names.end()) {
            notebook_.set_current_page(static_cast<int>(std::distance(names.begin(), match)));
        }
    }
}

void Window::build_header() {
    brand_icon_.set_from_resource("/com/linuxgamerlife/ppp/lgl-powerprofile-manager.png");
    brand_icon_.set_pixel_size(28);
    brand_icon_.add_css_class("brand-icon");
    wordmark_.add_css_class("wordmark");
    brand_.set_margin_end(12);
    brand_.append(brand_icon_);
    brand_.append(wordmark_);
    header_.pack_start(brand_);

    refresh_button_.set_tooltip_text("Refresh the active profile and profile list (F5)");
    header_.pack_start(refresh_button_);

    apply_button_.add_css_class("pp-primary");
    apply_button_.add_css_class("suggested-action");
    apply_button_.set_tooltip_text("Apply the selected tuned profile (Ctrl+Enter)");
    header_.pack_end(menu_button_);
    header_.pack_end(apply_button_);

    menu_button_.set_icon_name("open-menu-symbolic");
    menu_button_.set_tooltip_text("Main menu (F10)");
    header_.set_title_widget(*Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL));
}

void Window::build_pages() {
    overlay_.set_child(notebook_);
    notice_label_.set_wrap(true);
    notice_label_.set_max_width_chars(80);
    notice_label_.add_css_class("pp-notice");
    notice_revealer_.set_transition_type(Gtk::RevealerTransitionType::SLIDE_UP);
    notice_revealer_.set_halign(Gtk::Align::CENTER);
    notice_revealer_.set_valign(Gtk::Align::END);
    notice_revealer_.set_margin_bottom(16);
    notice_revealer_.set_can_target(false);
    notice_revealer_.set_child(notice_label_);
    overlay_.add_overlay(notice_revealer_);
    overlay_.set_hexpand(true);
    overlay_.set_vexpand(true);
    content_.append(overlay_);

    notebook_.append_page(build_status_page(), "Status");
    notebook_.append_page(build_profiles_page(), "Profiles");
    notebook_.append_page(build_log_page(), "Log");
    notebook_.append_page(build_reference_page(), "Reference");
    notebook_.append_page(build_setup_page(), "Setup");
}

Gtk::Widget& Window::build_status_page() {
    auto* body = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    body->add_css_class("body");

    auto& current = make_card("ACTIVE PROFILE");
    status_chip_.add_css_class("pp-chip");
    status_chip_.add_css_class("neutral");
    status_chip_.set_halign(Gtk::Align::START);
    current.append(status_chip_);
    auto* grid = Gtk::make_managed<Gtk::Grid>();
    grid->set_row_spacing(10);
    grid->set_column_spacing(12);
    add_info_row(*grid, 0, "Profile", profile_value_);
    add_info_row(*grid, 1, "Service", service_value_);
    current.append(*grid);
    body->append(current);

    auto& tools = make_card("TOOLS");
    auto* tools_grid = Gtk::make_managed<Gtk::Grid>();
    tools_grid->set_row_spacing(10);
    tools_grid->set_column_spacing(12);
    add_info_row(*tools_grid, 0, "tuned-adm installed", tuned_value_);
    add_info_row(*tools_grid, 1, "pkexec installed", polkit_value_);
    tuned_value_.set_text(tool_exists("tuned-adm") ? "Yes" : "No");
    polkit_value_.set_text(tool_exists("pkexec") ? "Yes" : "No");
    tools.append(*tools_grid);
    body->append(tools);

    auto& note = make_card("POWER-PROFILES-DAEMON");
    auto* explanation = &text_label(
        "When a tuned profile is applied it overrides power-profiles-daemon settings. If your "
        "profile is reverted, mask power-profiles-daemon so it stops competing with tuned.");
    explanation->add_css_class("muted");
    note.append(*explanation);
    auto* disable = Gtk::make_managed<Gtk::Label>("TO DISABLE");
    disable->set_halign(Gtk::Align::START);
    disable->add_css_class("reference-label");
    note.append(*disable);
    note.append(code_label("sudo systemctl mask power-profiles-daemon"));
    auto* enable = Gtk::make_managed<Gtk::Label>("TO RE-ENABLE");
    enable->set_halign(Gtk::Align::START);
    enable->add_css_class("reference-label");
    note.append(*enable);
    note.append(code_label("sudo systemctl unmask power-profiles-daemon"));
    body->append(note);

    return *body;
}

Gtk::Widget& Window::build_profiles_page() {
    auto* body = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    body->add_css_class("body");

    auto& card = make_card("TUNED PROFILES", true);
    auto* scroller = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroller->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    scroller->set_vexpand(true);
    scroller->add_css_class("profiles-scroller");
    profile_list_.set_margin_top(10);
    profile_list_.set_margin_bottom(10);
    profile_list_.set_margin_start(10);
    profile_list_.set_margin_end(10);
    scroller->set_child(profile_list_);
    card.append(*scroller);
    body->append(card);

    return *body;
}

Gtk::Widget& Window::build_log_page() {
    auto* body = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
    body->add_css_class("body");

    auto& card = make_card("COMMAND LOG", true);
    auto* scroller = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroller->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scroller->set_vexpand(true);
    log_view_.set_editable(false);
    log_view_.set_cursor_visible(false);
    log_view_.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
    log_view_.add_css_class("mono");
    log_view_.add_css_class("log-view");
    scroller->set_child(log_view_);
    card.append(*scroller);

    auto* actions = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actions->set_halign(Gtk::Align::END);
    actions->set_margin_top(8);
    actions->set_margin_bottom(12);
    actions->set_margin_start(14);
    actions->set_margin_end(14);
    auto* clear = Gtk::make_managed<Gtk::Button>("Clear log");
    clear->add_css_class("pp-small");
    clear->signal_clicked().connect([this] { log_view_.get_buffer()->set_text(""); });
    actions->append(*clear);
    card.append(*actions);
    body->append(card);

    return *body;
}

Gtk::Widget& Window::build_reference_page() {
    auto* body = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    body->add_css_class("body");

    auto& card = make_card("PROFILE REFERENCE", true);
    auto* scroller = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroller->set_vexpand(true);
    auto* list = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    list->set_margin_top(2);
    list->set_margin_bottom(10);
    list->set_margin_start(10);
    list->set_margin_end(10);
    for (const auto& entry : builtin_profiles()) {
        list->append(reference_item(entry));
    }
    scroller->set_child(*list);
    card.append(*scroller);
    body->append(card);

    return *body;
}

Gtk::Widget& Window::build_setup_page() {
    auto* body = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    body->add_css_class("body");

    auto& card = make_card("TUNED SETUP");
    auto* text = &text_label(
        "This application uses tuned and tuned-adm. If they are not installed, install the "
        "tuned package with your distribution's package manager, enable the service, then "
        "restart this application.");
    card.append(*text);

    auto* fedora = Gtk::make_managed<Gtk::Label>("FEDORA / RHEL / CENTOS");
    fedora->set_halign(Gtk::Align::START);
    fedora->add_css_class("reference-label");
    card.append(*fedora);
    card.append(code_label("sudo dnf install tuned"));
    card.append(code_label("sudo systemctl enable --now tuned"));

    auto* others = Gtk::make_managed<Gtk::Label>("OTHER DISTRIBUTIONS");
    others->set_halign(Gtk::Align::START);
    others->add_css_class("reference-label");
    card.append(*others);
    auto* other_text = &text_label(
        "Install the tuned package for your distribution and make sure the tuned service is "
        "enabled, then restart this application.");
    other_text->add_css_class("muted");
    card.append(*other_text);

    body->append(card);
    return *body;
}

void Window::install_actions() {
    refresh_action_ = Gio::SimpleAction::create("refresh");
    refresh_action_->set_enabled(false);
    refresh_action_->signal_activate().connect(
        [this](const Glib::VariantBase&) { refresh_all(); });
    add_action(refresh_action_);

    apply_action_ = Gio::SimpleAction::create("apply");
    apply_action_->set_enabled(false);
    apply_action_->signal_activate().connect(
        [this](const Glib::VariantBase&) { apply_profile(); });
    add_action(apply_action_);

    add_action("help", sigc::mem_fun(*this, &Window::show_help));
    add_action("about", sigc::mem_fun(*this, &Window::show_about));

    system_title_action_ =
        Gio::SimpleAction::create_bool("system-title-bar", settings_.system_title_bar());
    system_title_action_->signal_activate().connect([this](const Glib::VariantBase&) {
        bool current = false;
        system_title_action_->get_state(current);
        const bool enabled = !current;
        system_title_action_->set_state(Glib::Variant<bool>::create(enabled));
        settings_.set_system_title_bar(enabled);
        apply_title_bar(enabled, true);
    });
    add_action(system_title_action_);

    auto menu = Gio::Menu::create();
    auto operations = Gio::Menu::create();
    operations->append("Refresh", "win.refresh");
    operations->append("Apply Profile", "win.apply");
    menu->append_section(operations);
    auto appearance = Gio::Menu::create();
    appearance->append("Use System Title Bar", "win.system-title-bar");
    menu->append_section(appearance);
    auto information = Gio::Menu::create();
    information->append("Help", "win.help");
    information->append("About LGL Power Profile Manager", "win.about");
    menu->append_section(information);
    auto quit = Gio::Menu::create();
    quit->append("Quit", "app.quit");
    menu->append_section(quit);
    menu_model_ = menu;
    ensure_main_menu();

    application_->set_accel_for_action("win.refresh", "F5");
    application_->set_accel_for_action("win.apply", "<Ctrl>Return");
    application_->set_accel_for_action("win.help", "F1");
    application_->set_accel_for_action("win.system-title-bar", "<Ctrl><Shift>M");

    install_menu_key_controller(*this);
}

void Window::bind_header_actions() {
    refresh_button_.set_action_name("win.refresh");
    apply_button_.set_action_name("win.apply");
}

void Window::install_menu_key_controller(Gtk::Widget& widget) {
    auto controller = Gtk::EventControllerKey::create();
    controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
    controller->signal_key_pressed().connect(sigc::mem_fun(*this, &Window::on_menu_key_pressed),
                                             false);
    controller->signal_key_released().connect(sigc::mem_fun(*this, &Window::on_menu_key_released));
    widget.add_controller(controller);
}

bool Window::on_menu_key_pressed(const guint keyval, guint, Gdk::ModifierType) {
    if (keyval == GDK_KEY_F10) {
        alt_only_ = false;
        if (!f10_down_) {
            f10_down_ = true;
            toggle_main_menu();
        }
        return true;
    }
    alt_only_ = keyval == GDK_KEY_Alt_L || keyval == GDK_KEY_Alt_R;
    return false;
}

void Window::on_menu_key_released(const guint keyval, guint, Gdk::ModifierType) {
    if (keyval == GDK_KEY_F10) {
        f10_down_ = false;
        return;
    }
    const bool is_alt = keyval == GDK_KEY_Alt_L || keyval == GDK_KEY_Alt_R;
    if (is_alt && alt_only_) {
        toggle_main_menu();
    }
    alt_only_ = false;
}

void Window::ensure_main_menu() {
    if (!menu_model_ || menu_button_.get_menu_model() != nullptr) {
        return;
    }
    menu_button_.set_menu_model(menu_model_);
    if (auto* popover = menu_button_.get_popover()) {
        popover->signal_closed().connect([this] { menu_open_ = false; });
        install_menu_key_controller(*popover);
    }
}

void Window::open_main_menu() {
    reset_main_menu_popover();
    ensure_main_menu();
    menu_open_ = true;
    menu_button_.popup();
}

void Window::close_main_menu() {
    menu_open_ = false;
    menu_button_.popdown();
}

void Window::toggle_main_menu() {
    if (menu_open_) {
        close_main_menu();
        Glib::signal_idle().connect_once([this] { focus_primary_input(); });
    } else {
        open_main_menu();
    }
}

void Window::reset_main_menu_popover() {
    close_main_menu();
    menu_button_.unset_menu_model();
}

void Window::on_window_hidden() {
    f10_down_ = false;
    alt_only_ = false;
    if (!tray_available_) {
        return;
    }
    reset_main_menu_popover();
    Glib::signal_idle().connect_once([this] {
        if (!get_visible()) {
            Gtk::Widget::unrealize();
        }
    });
}

void Window::apply_title_bar(const bool system_title_bar, const bool runtime) {
    if (runtime) {
        application_->hold();
        set_visible(false);
        Gtk::Widget::unrealize();
    }
    if (system_title_bar) {
        unset_titlebar();
        header_.set_show_title_buttons(false);
        content_.prepend(header_);
    } else {
        if (header_.get_parent() == &content_) {
            content_.remove(header_);
        }
        header_.set_show_title_buttons(true);
        set_titlebar(header_);
    }
    if (runtime) {
        set_application(application_);
        bind_header_actions();
        present();
        focus_primary_input();
        application_->release();
    }
}

void Window::focus_primary_input() {
    for (auto& row : profile_rows_) {
        if (row.button->get_active()) {
            row.button->grab_focus();
            return;
        }
    }
    if (!profile_rows_.empty()) {
        profile_rows_.front().button->grab_focus();
    }
}

void Window::show_help() {
    if (!help_window_) {
        help_window_ = std::make_unique<HelpWindow>(*this);
    }
    help_window_->set_mono_family(theme_watcher_ ? theme_watcher_->mono_family()
                                                 : "Adwaita Mono");
    help_window_->present();
}

void Window::show_about() {
    if (!about_dialog_) {
        about_dialog_ = std::make_unique<Gtk::AboutDialog>();
        about_dialog_->set_program_name("LGL Power Profile Manager");
        about_dialog_->set_version(PPP_VERSION);
        about_dialog_->set_comments(
            "Switch between tuned power profiles on Fedora and RHEL-based systems.\n\n"
            "Built with GTK 4.");
        about_dialog_->set_website("https://github.com/linuxgamerlife/lgl-powerprofile-manager");
        about_dialog_->set_website_label("Project page and issue tracker");
        about_dialog_->set_authors({"LinuxGamerLife https://www.youtube.com/@linuxgamerlife"});
        about_dialog_->add_credit_section(
            "Support", {"Support development on Ko-fi https://ko-fi.com/G2G3V70LW"});
        about_dialog_->set_license_type(Gtk::License::MIT_X11);
        about_dialog_->set_logo(Gdk::Texture::create_from_resource(
            "/com/linuxgamerlife/ppp/lgl-powerprofile-manager.png"));
        about_dialog_->set_transient_for(*this);
        about_dialog_->set_modal(true);
        about_dialog_->set_hide_on_close(true);
    }
    about_dialog_->present();
}

void Window::show_notice(const std::string& text, const std::string& status, const int seconds) {
    notice_label_.set_text(text);
    for (const auto& name : {"positive", "neutral", "negative"}) {
        notice_label_.remove_css_class(name);
    }
    if (!status.empty()) {
        notice_label_.add_css_class(status);
    }
    notice_revealer_.set_reveal_child(true);
    if (notice_timer_.connected()) {
        notice_timer_.disconnect();
    }
    if (seconds > 0) {
        notice_timer_ = Glib::signal_timeout().connect_seconds(
            [this] {
                notice_revealer_.set_reveal_child(false);
                return false;
            },
            seconds);
    }
}

void Window::append_log(const std::string& text, const std::string&) {
    if (text.empty()) {
        return;
    }
    auto buffer = log_view_.get_buffer();
    buffer->insert(buffer->end(), "[" + timestamp() + "] " + text + "\n");
    if (buffer->get_char_count() > 100000) {
        auto end = buffer->get_iter_at_offset(20000);
        buffer->erase(buffer->begin(), end);
    }
}

bool Window::tool_exists(const char* name) const {
    gchar* executable = g_find_program_in_path(name);
    const bool found = executable != nullptr;
    g_free(executable);
    return found;
}

void Window::refresh_status() {
    request_status(true);
}

void Window::refresh_status_quietly() {
    request_status(false);
}

void Window::refresh_all() {
    refresh_profiles();
    refresh_status();
}

void Window::request_status(const bool sync_selection) {
    if (status_request_active_ || !ready_) {
        return;
    }
    status_request_active_ = true;
    const std::weak_ptr<int> lifetime = lifetime_;
    runner_.run(
        {"tuned-adm", "active"},
        [this, lifetime, sync_selection](const CommandResult& result) {
            if (lifetime.expired()) {
                return;
            }
            status_request_active_ = false;
            if (!result.launch_error.empty() || result.timed_out || result.exit_code != 0) {
                update_status({}, sync_selection);
                return;
            }
            update_status(parse_active_profile(result.standard_output), sync_selection);
        },
        5);
}

void Window::refresh_profiles() {
    if (profiles_request_active_ || !ready_) {
        return;
    }
    profiles_request_active_ = true;
    const std::weak_ptr<int> lifetime = lifetime_;
    runner_.run(
        {"tuned-adm", "list"},
        [this, lifetime](const CommandResult& result) {
            if (lifetime.expired()) {
                return;
            }
            profiles_request_active_ = false;
            std::vector<std::string> names;
            if (result.exit_code == 0 && result.launch_error.empty() && !result.timed_out) {
                names = parse_profile_list(result.standard_output);
            }
            if (names.empty()) {
                for (const auto& entry : builtin_profiles()) {
                    names.push_back(entry.name);
                }
                append_log("Could not read the tuned profile list; using the built-in list.");
            }
            const std::string desired =
                active_profile_.empty() ? selected_profile() : active_profile_;
            populate_profiles(order_profiles(names), desired);
        },
        10);
}

void Window::populate_profiles(const std::vector<std::string>& profiles,
                               const std::string& select) {
    while (auto* child = profile_list_.get_first_child()) {
        profile_list_.remove(*child);
    }
    profile_rows_.clear();

    Gtk::CheckButton* first = nullptr;
    Gtk::CheckButton* chosen = nullptr;
    for (const auto& name : profiles) {
        const ProfileInfo info = find_profile(name);
        auto* button = Gtk::make_managed<Gtk::CheckButton>();
        button->add_css_class("profile-row");
        button->set_child(profile_row_content(info));
        if (!info.best_for.empty()) {
            button->set_tooltip_text("Best for: " + info.best_for);
        }
        if (first == nullptr) {
            first = button;
        } else {
            button->set_group(*first);
        }
        profile_list_.append(*button);
        profile_rows_.push_back({name, button});
        if (name == select) {
            chosen = button;
        }
    }

    if (chosen != nullptr) {
        chosen->set_active(true);
    } else if (first != nullptr) {
        first->set_active(true);
    }
    set_busy(busy_);
}

void Window::select_profile(const std::string& name) {
    for (auto& row : profile_rows_) {
        if (row.name == name) {
            row.button->set_active(true);
            return;
        }
    }
}

std::string Window::selected_profile() const {
    for (const auto& row : profile_rows_) {
        if (row.button->get_active()) {
            return row.name;
        }
    }
    return {};
}

void Window::apply_profile() {
    if (!ready_ || busy_) {
        return;
    }
    const std::string profile = selected_profile();
    if (profile.empty()) {
        show_notice("Select a profile first.", "neutral");
        return;
    }
    if (!valid_profile_name(profile)) {
        append_log("Invalid profile name: " + profile);
        show_notice("The selected profile name is not valid.", "negative", 10);
        return;
    }
    run_mutation({"pkexec", "tuned-adm", "profile", profile},
                 "Applying profile \"" + profile + "\"");
}

void Window::run_mutation(const std::vector<std::string>& arguments,
                          const std::string& description) {
    set_busy(true);
    append_log(description);
    show_notice(description + "...", "neutral", 0);
    const std::weak_ptr<int> lifetime = lifetime_;
    const std::string command = arguments.front();
    runner_.run(
        arguments,
        [this, description, command, lifetime](const CommandResult& result) {
            if (lifetime.expired()) {
                return;
            }
            set_busy(false);
            if (!result.standard_output.empty()) {
                append_log(result.standard_output);
            }
            if (!result.standard_error.empty()) {
                append_log(result.standard_error);
            }
            if (result.timed_out) {
                show_notice(description + " timed out.", "negative", 10);
            } else if (!result.launch_error.empty()) {
                show_notice("Could not run " + command + ": " + result.launch_error, "negative",
                            10);
            } else if (result.exit_code == 126 || result.exit_code == 127) {
                show_notice("Authorization cancelled.", "neutral", 10);
            } else if (result.exit_code != 0) {
                show_notice(description + " failed (exit " + std::to_string(result.exit_code) +
                                ").",
                            "negative", 10);
            } else {
                show_notice(description + " completed.", "positive");
            }
            refresh_status();
        },
        60);
}

void Window::update_status(const std::string& profile, const bool sync_selection) {
    active_profile_ = profile;
    const bool active = !profile.empty();
    status_chip_.set_text(active ? "ACTIVE" : "UNKNOWN");
    status_chip_.remove_css_class(active ? "neutral" : "positive");
    status_chip_.add_css_class(active ? "positive" : "neutral");
    profile_value_.set_text(active ? profile : "-");
    service_value_.set_text(active ? "Running" : "Unknown");
    if (profile_callback_) {
        profile_callback_(profile);
    }
    if (sync_selection && active) {
        select_profile(profile);
    }
}

void Window::set_busy(const bool busy) {
    busy_ = busy;
    const bool enabled = ready_ && !busy_;
    if (refresh_action_) {
        refresh_action_->set_enabled(enabled);
    }
    if (apply_action_) {
        apply_action_->set_enabled(enabled && !profile_rows_.empty());
    }
}

void Window::enter_setup_mode() {
    ready_ = false;
    set_busy(busy_);
    notebook_.set_current_page(4);
    for (int page = 0; page < 3; ++page) {
        if (auto* widget = notebook_.get_nth_page(page)) {
            widget->set_sensitive(false);
        }
    }
    show_notice("tuned-adm was not found; see the Setup tab.", "neutral", 10);
}

void Window::bring_to_front() {
    reset_main_menu_popover();
    ensure_main_menu();
    present();
}

void Window::set_tray_available(const bool available) {
    tray_available_ = available;
    set_hide_on_close(available);
    if (start_minimized_) {
        start_minimized_ = false;
        if (available) {
            set_visible(false);
        } else {
            present();
            show_notice("No system tray host was found; showing the window instead.", "neutral",
                        10);
        }
    }
}

void Window::set_profile_callback(std::function<void(const std::string&)> callback) {
    profile_callback_ = std::move(callback);
    if (profile_callback_) {
        profile_callback_(active_profile_);
    }
}

void Window::show_main_menu() {
    present();
    open_main_menu();
}

void Window::refresh_from_tray() {
    refresh_status();
}

} // namespace ppp
