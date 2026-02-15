#include "daedalus/platform/windows/selectors.h"

#include "daedalus/strings/utils.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <shobjidl.h> // For IFileDialog

namespace dae::selector
{
auto open_windows_folder_dialogue() -> std::optional<std::string>
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        return std::nullopt;
    }

    std::wstring wFolderPath;

    // Create the FileOpenDialog object
    IFileDialog* pFileDialog = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
    if (SUCCEEDED(hr))
    {
        // Set the options for the dialog
        DWORD options{};
        pFileDialog->GetOptions(&options);
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        // Show the dialog
        hr = pFileDialog->Show(NULL);
        if (SUCCEEDED(hr))
        {
            // Get the result
            IShellItem* pItem = nullptr;
            hr = pFileDialog->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                // Get the folder path
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    wFolderPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }

    CoUninitialize();

    if (FAILED(hr))
    {
        return std::nullopt;
    }

    return dae::strings::from_wide(wFolderPath);
}
} // namespace dae::selector
