#pragma once

#include "core/settings.h"
#include "ui/command_runner.h"
#include "ui/themewatcher.h"

#include <gtkmm.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ppp {

class HelpWindow;

class Window final : public Gtk::ApplicationWindow {
  public:
    Window(const Glib::RefPtr<Gtk::Application>& application, bool start_minimized);
    ~Window() override;

    void bring_to_front();
    void set_tray_available(bool available);
    void set_profile_callback(std::function<void(const std::string&)> callback);
    void show_main_menu();
    void refresh_from_tray();

  private:
    struct ProfileRow {
        std::string name;
        Gtk::CheckButton* button{nullptr};
    };

    void build_header();
    void build_pages();
    Gtk::Widget& build_status_page();
    Gtk::Widget& build_profiles_page();
    Gtk::Widget& build_log_page();
    Gtk::Widget& build_reference_page();
    Gtk::Widget& build_setup_page();
    void install_actions();
    void bind_header_actions();
    void ensure_main_menu();
    void open_main_menu();
    void close_main_menu();
    void toggle_main_menu();
    void reset_main_menu_popover();
    void on_window_hidden();
    void install_menu_key_controller(Gtk::Widget& widget);
    bool on_menu_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_menu_key_released(guint keyval, guint keycode, Gdk::ModifierType state);
    void apply_title_bar(bool system_title_bar, bool runtime);
    void focus_primary_input();
    void show_help();
    void show_about();
    void show_notice(const std::string& text, const std::string& status = {}, int seconds = 5);
    void append_log(const std::string& text, const std::string& status = {});
    void refresh_status();
    void refresh_status_quietly();
    void refresh_all();
    void request_status(bool sync_selection);
    void refresh_profiles();
    void populate_profiles(const std::vector<std::string>& profiles, const std::string& select);
    void select_profile(const std::string& name);
    void apply_profile();
    void run_mutation(const std::vector<std::string>& arguments, const std::string& description);
    void update_status(const std::string& profile, bool sync_selection);
    void set_busy(bool busy);
    void enter_setup_mode();
    std::string selected_profile() const;
    bool tool_exists(const char* name) const;

    Settings settings_;
    CommandRunner runner_;
    std::unique_ptr<ThemeWatcher> theme_watcher_;
    Glib::RefPtr<Gtk::Application> application_;
    Glib::RefPtr<Gio::SimpleAction> refresh_action_;
    Glib::RefPtr<Gio::SimpleAction> apply_action_;
    Glib::RefPtr<Gio::SimpleAction> system_title_action_;
    Glib::RefPtr<Gio::MenuModel> menu_model_;
    std::unique_ptr<HelpWindow> help_window_;
    std::unique_ptr<Gtk::AboutDialog> about_dialog_;

    Gtk::Box content_{Gtk::Orientation::VERTICAL};
    Gtk::HeaderBar header_;
    Gtk::Box brand_{Gtk::Orientation::HORIZONTAL, 10};
    Gtk::Image brand_icon_;
    Gtk::Label wordmark_{"LGL POWER PROFILE MANAGER"};
    Gtk::Button refresh_button_{"Refresh"};
    Gtk::Button apply_button_{"Apply Profile"};
    Gtk::MenuButton menu_button_;
    Gtk::Overlay overlay_;
    Gtk::Notebook notebook_;
    Gtk::Revealer notice_revealer_;
    Gtk::Label notice_label_;

    Gtk::Label status_chip_{"UNKNOWN"};
    Gtk::Label profile_value_{"-"};
    Gtk::Label service_value_{"Unknown"};
    Gtk::Label tuned_value_{"No"};
    Gtk::Label polkit_value_{"No"};
    Gtk::Box profile_list_{Gtk::Orientation::VERTICAL, 6};
    Gtk::TextView log_view_;

    std::vector<ProfileRow> profile_rows_;
    sigc::connection refresh_timer_;
    sigc::connection notice_timer_;
    bool ready_{false};
    bool busy_{false};
    bool status_request_active_{false};
    bool profiles_request_active_{false};
    bool tray_available_{false};
    bool menu_open_{false};
    bool alt_only_{false};
    bool f10_down_{false};
    bool start_minimized_{false};
    std::string active_profile_;
    std::function<void(const std::string&)> profile_callback_;
    std::shared_ptr<int> lifetime_{std::make_shared<int>(0)};
};

} // namespace ppp
