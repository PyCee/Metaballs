#include "vulkan_instance.hpp"
#include <iostream>
#include <dlfcn.h>

using namespace MTB;

#if defined(MTB_DEBUG)
VKAPI_ATTR VkBool32 VKAPI_CALL vk_validation_callback
(VkDebugReportFlagsEXT flags,
 VkDebugReportObjectTypeEXT objectType,
 uint64_t object,
 size_t location,
 int32_t messageCode,
 const char* pLayerPrefix,
 const char* pMessage,
 void* pUserData){
  std::cout << "VALIDATION_ERROR::" << pMessage << std::endl;
  return VK_FALSE;
}
VkDebugReportCallbackEXT debug_report_callback;

PFN_vkGetInstanceProcAddr GetInstanceProcAddr = VK_NULL_HANDLE;
PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDebugReportMessageEXT DebugReportMessage = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
#if defined(_WIN32)
static HWMODULE vulkan_library;
#define LOAD_VULKAN() (vulkan_library = LoadLibrary("vulkan-1.dll"))
#define FREE_LIBRARY(library) FreeLibrary(library)
#define LOAD_LIBRARY_FUNCTION(library, fun_string) GetProcAddress(library, fun_string)
#elif defined(__unix__)
static void *vulkan_library;
#define LOAD_VULKAN() (vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW))
#define FREE_LIBRARY(library) dlclose(library)
#define LOAD_LIBRARY_FUNCTION(library, fun_string) dlsym(library, fun_string)
#endif /* __unix__ */
#endif /* MTB_DEBUG */


Vulkan_Instance::Vulkan_Instance(){
  unsigned int i,
    j;
  std::vector<std::string> extensions,
    layers;
  extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
  extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__unix__)
  extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
  
#if defined(MTB_DEBUG)
  // If the program is compiled with 'make debug'
  // Add the validation extension and layer
  extensions.push_back("VK_EXT_debug_report");
  layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif /* MTB_DEBUG */
  
  VkApplicationInfo application_create_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = "Metaball",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "Metaball_Custom",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0
  };

  char const *char_extensions[extensions.size()];
  char const *char_layers[layers.size()];
  
  unsigned int extension_count;
  vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(NULL, &extension_count,
					 available_extensions.data());
  for(i = 0; i < extensions.size(); ++i){
    bool extension_exists = false;
    for(j = 0; j < available_extensions.size(); ++j){
      if(extensions[i] == available_extensions[j].extensionName){
	extension_exists = true;
      }
    }
    if(!extension_exists){
      std::cout << "Extension \'" << extensions[i] << "\' does not exist" << std::endl;
    }
    char_extensions[i] = extensions.data()[i].c_str();
  }
  
  unsigned int layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
  for(i = 0; i < layers.size(); ++i){
    bool layer_exists = false;
    for(j = 0; j < available_layers.size(); ++j){
      if(layers[i] == available_layers[j].layerName){
	layer_exists = true;
      }
    }
    if(!layer_exists){
      std::cout << "Layer \'" << layers[i] << "\' does not exist" << std::endl;
    }
    char_layers[i] =  layers.data()[i].c_str();
  }
  VkInstanceCreateInfo instance_create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .pApplicationInfo = &application_create_info,
    .enabledLayerCount = (unsigned int)layers.size(),
    .ppEnabledLayerNames = (char const* const*)char_layers,
    .enabledExtensionCount = (unsigned int)extensions.size(),
    .ppEnabledExtensionNames = (char const* const*)char_extensions
  };
  this->m_instance = VK_NULL_HANDLE;
  if(vkCreateInstance(&instance_create_info, NULL,
		      &this->m_instance) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create vulkan instance" << std::endl;
  } else {
    std::cout << "Created vulkan instance" << std::endl;
  }
  
#if defined(MTB_DEBUG)
  LOAD_VULKAN();
  if(NULL == vulkan_library){
    std::cout << "ERROR::Failed to load vulkan library (for debugging)" << std::endl;
  }
  GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
    LOAD_LIBRARY_FUNCTION(vulkan_library, "vkGetInstanceProcAddr");
  CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)
    GetInstanceProcAddr(this->m_instance, "vkCreateDebugReportCallbackEXT");
  DebugReportMessage = (PFN_vkDebugReportMessageEXT)
    GetInstanceProcAddr(this->m_instance, "vkDebugReportMessageEXT");
  DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)
    GetInstanceProcAddr(this->m_instance, "vkDestroyDebugReportCallbackEXT");

  // Create the debug callback
  VkDebugReportCallbackCreateInfoEXT callback_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
    .pNext = NULL,
    .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
    VK_DEBUG_REPORT_WARNING_BIT_EXT |
    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
    .pfnCallback = &vk_validation_callback,
    .pUserData = NULL
  };
  if(CreateDebugReportCallback(this->m_instance, &callback_create_info, NULL,
			       &debug_report_callback) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create debug report callback" << std::endl;
  } else{
    std::cout << "Created debug report callback" << std::endl;
  }
#endif
}
Vulkan_Instance::~Vulkan_Instance(){
#if defined(MTB_DEBUG)
  DestroyDebugReportCallback(this->m_instance, debug_report_callback,
			     VK_NULL_HANDLE);
  debug_report_callback = VK_NULL_HANDLE;
  FREE_LIBRARY(vulkan_library);
#endif
  vkDestroyInstance(this->m_instance, NULL);
}
