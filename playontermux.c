#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

// Function to run shell commands with Wine environment variables
void run_command(const char *cmd) {
    if (fork() == 0) { // Child process
        setenv("DXVK_HUD", "fps,gpuload,devinfo,api", 1);
        setenv("MESA_LOADER_DRIVER_OVERRIDE", "zink", 1);
        execlp("sh", "sh", "-c", cmd, NULL);
        _exit(127); // Exit if execlp fails
    }
}

// Install EXE from /sdcard/
void install_app(GtkWidget *widget, gpointer data) {
    run_command("FILE=$(zenity --file-selection --title='Select Windows Installer' --filename='/sdcard/' --file-filter='*.exe'); if [ -n \"$FILE\" ]; then wine \"$FILE\"; fi");
}

// Uninstall installed applications
void uninstall_app(GtkWidget *widget, gpointer data) {
    run_command("wine uninstaller");
}

// Open Wine desktop shell
void open_wine_desktop(GtkWidget *widget, gpointer data) {
    run_command("wine explorer /desktop=shell explorer");
}

// Open Wine task manager inside Wine (not Termux process)
void open_task_manager(GtkWidget *widget, gpointer data) {
    run_command("wine start taskmgr");
}

// Open Wine file explorer
void open_file_explorer(GtkWidget *widget, gpointer data) {
    run_command("wine explorer");
}

// Open Wine configuration
void open_winecfg(GtkWidget *widget, gpointer data) {
    run_command("winecfg");
}

// Open Wine registry editor
void open_regedit(GtkWidget *widget, gpointer data) {
    run_command("wine regedit");
}

// Run Winetricks
void open_winetricks(GtkWidget *widget, gpointer data) {
    run_command("winetricks");
}

// Install DXVK with correct DLL copy
void install_dxvk(GtkWidget *widget, gpointer data) {
    run_command(
        "DXVK_DIR=\"$HOME/.wine/drive_c/dxvk\"; "
        "mkdir -p \"$DXVK_DIR\"; "
        "VERSION=$(zenity --list --title='Select DXVK Version' --column='Version' '2.3' '2.2' '1.10.3'); "
        "if [ -n \"$VERSION\" ]; then "
        "wget -qO \"$DXVK_DIR/dxvk.tar.gz\" \"https://github.com/doitsujin/dxvk/releases/download/v$VERSION/dxvk-$VERSION.tar.gz\" && "
        "tar -xf \"$DXVK_DIR/dxvk.tar.gz\" -C \"$DXVK_DIR\" --strip-components=1 && "
        "cp \"$DXVK_DIR/x64\"/*.dll \"$HOME/.wine/drive_c/windows/system32/\" && "
        "cp \"$DXVK_DIR/x32\"/*.dll \"$HOME/.wine/drive_c/windows/syswow64/\" && "
        "rm \"$DXVK_DIR/dxvk.tar.gz\"; "
        "zenity --info --text='DXVK $VERSION Installed Successfully'; "
        "else zenity --error --text='DXVK Installation Canceled'; "
        "fi"
    );
}

// Apply native DLL overrides for DXVK
void set_dxvk_overrides(GtkWidget *widget, gpointer data) {
    run_command(
        "wine reg add \"HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides\" "
        "/v d3d8 /t REG_SZ /d native /f && "
        "wine reg add \"HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides\" "
        "/v d3d9 /t REG_SZ /d native /f && "
        "wine reg add \"HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides\" "
        "/v d3d10core /t REG_SZ /d native /f && "
        "wine reg add \"HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides\" "
        "/v d3d11 /t REG_SZ /d native /f && "
        "wine reg add \"HKEY_CURRENT_USER\\Software\\Wine\\DllOverrides\" "
        "/v dxgi /t REG_SZ /d native /f && "
        "zenity --info --text='DXVK DLL Overrides Set Successfully'"
    );
}

// Apply Custom Styling
void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    const gchar *css_data =
        "window { background-color: #E5E5E5; padding: 10px; }"
        "button { font-size: 14px; color: #0A0A0A; background-color: #FFFFFF; padding: 12px; border-radius: 5px; border: 1px solid #444; }"
        "button:hover { background-color: #CCCCCC; }"
        "label { color: #000000; font-size: 14px; font-weight: bold; }";

    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
}

// Create PlayOnTermux UI
void create_ui() {
    GtkWidget *window, *vbox, *button_install, *button_uninstall, *button_shell, *button_taskmgr;
    GtkWidget *button_explorer, *button_exit, *button_winecfg, *button_regedit, *button_winetricks, *button_dxvk, *button_overrides;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "PlayOnTermux");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    button_install = gtk_button_new_with_label("📂 Install App");
    g_signal_connect(button_install, "clicked", G_CALLBACK(install_app), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_install, TRUE, TRUE, 5);

    button_uninstall = gtk_button_new_with_label("🗑 Uninstall App");
    g_signal_connect(button_uninstall, "clicked", G_CALLBACK(uninstall_app), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_uninstall, TRUE, TRUE, 5);

    button_shell = gtk_button_new_with_label("🖥 Open Desktop Shell");
    g_signal_connect(button_shell, "clicked", G_CALLBACK(open_wine_desktop), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_shell, TRUE, TRUE, 5);

    button_taskmgr = gtk_button_new_with_label("📊 Open Task Manager");
    g_signal_connect(button_taskmgr, "clicked", G_CALLBACK(open_task_manager), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_taskmgr, TRUE, TRUE, 5);

    button_explorer = gtk_button_new_with_label("📁 Open File Explorer");
    g_signal_connect(button_explorer, "clicked", G_CALLBACK(open_file_explorer), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_explorer, TRUE, TRUE, 5);

    button_dxvk = gtk_button_new_with_label("🔧 Install DXVK");
    g_signal_connect(button_dxvk, "clicked", G_CALLBACK(install_dxvk), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_dxvk, TRUE, TRUE, 5);

    button_overrides = gtk_button_new_with_label("🔄 Set DXVK Overrides");
    g_signal_connect(button_overrides, "clicked", G_CALLBACK(set_dxvk_overrides), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_overrides, TRUE, TRUE, 5);

    button_winetricks = gtk_button_new_with_label("🎛 Run Winetricks");
    g_signal_connect(button_winetricks, "clicked", G_CALLBACK(open_winetricks), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_winetricks, TRUE, TRUE, 5);

    apply_css();
    gtk_widget_show_all(window);
    gtk_main();
}

int main() {
    create_ui();
    return 0;
}
