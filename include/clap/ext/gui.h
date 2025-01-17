#pragma once

#include "../plugin.h"

/// @page GUI
///
/// This extension is splet in two interfaces:
/// - `gui` which is the generic part
/// - `gui_XXX` which is the platform specific interfaces; @see clap_gui_win32.
///
/// Showing the GUI works as follow:
/// 1. clap_plugin_gui->create(), allocates gui resources
/// 2. clap_plugin_gui->set_scale()
/// 3. clap_plugin_gui->get_size(), gets initial size
/// 4. clap_plugin_gui_win32->embed(), or any other platform specific interface
/// 5. clap_plugin_gui->show()
/// 6. clap_plugin_gui->hide()/show() ...
/// 7. clap_plugin_gui->destroy() when done with the gui
///
/// Resizing the window (initiated by the plugin):
/// 1. Plugins calls clap_host_gui->request_resize()
/// 2. If the host returns true the new size is accepted,
///    the host doesn't have to call clap_plugin_gui->set_size().
///    If the host returns false, the new size is rejected.
///
/// Resizing the window (drag):
/// 1. Only possible if clap_plugin_gui->can_resize() returns true
/// 2. Mouse drag -> new_size
/// 3. clap_plugin_gui->round_size(new_size) -> working_size
/// 4. clap_plugin_gui->set_size(working_size)

static CLAP_CONSTEXPR const char CLAP_EXT_GUI[] = "clap.gui";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_plugin_gui {
   // Create and allocate all resources necessary for the gui.
   // After this call, the GUI is ready to be shown but it is not yet visible.
   // [main-thread]
   bool (*create)(const clap_plugin_t *plugin);

   // Free all resources associated with the gui.
   // [main-thread]
   void (*destroy)(const clap_plugin_t *plugin);

   // Set the absolute GUI scaling factor, and override any OS info.
   // If the plugin does not provide this function, then it should work out the scaling factor
   // itself by querying the OS directly.
   //
   // Return false if the plugin can't apply the scaling; true on success.
   // [main-thread,optional]
   bool (*set_scale)(const clap_plugin_t *plugin, double scale);

   // Get the current size of the plugin UI, in physical pixels.
   // clap_plugin_gui->create() must have been called prior to asking the size.
   // [main-thread]
   bool (*get_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // [main-thread]
   bool (*can_resize)(const clap_plugin_t *plugin);

   // If the plugin gui is resizable, then the plugin will calculate the closest
   // usable size to the given arguments.
   // The scaling is applied.
   // This method does not change the size.
   //
   // [main-thread]
   void (*round_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // Sets the window size
   // Returns true if the size is supported.
   // [main-thread]
   bool (*set_size)(const clap_plugin_t *plugin, uint32_t width, uint32_t height);

   // Show the window.
   // [main-thread]
   void (*show)(const clap_plugin_t *plugin);

   // Hide the window, this method do not free the resources, it just hides
   // the window content. Yet it maybe a good idea to stop painting timers.
   // [main-thread]
   void (*hide)(const clap_plugin_t *plugin);
} clap_plugin_gui_t;

typedef struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true if the new size is accepted, false otherwise.
    * The host doesn't have to call set_size().
    * [main-thread] */
   bool (*request_resize)(const clap_host_t *host, uint32_t width, uint32_t height);

   /* Request the host to show the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   bool (*request_show)(const clap_host_t *host);

   /* Request the host to hide the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   bool (*request_hide)(const clap_host_t *host);
} clap_host_gui_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif