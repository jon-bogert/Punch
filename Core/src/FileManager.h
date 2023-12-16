#pragma once
#include <string>

#include <Windows.h>
#include <shobjidl.h> 

class FileManager final
{
    FileManager() {}
    static FileManager& Get() { static FileManager instance; return instance; }

public:
    ~FileManager() = default;
    FileManager(const FileManager& other) = delete;
    FileManager(const FileManager&& other) = delete;
    FileManager operator=(const FileManager& other) = delete;
    FileManager operator=(const FileManager&& other) = delete;

    static std::wstring GetFile(const std::string start)
    {
        COMDLG_FILTERSPEC fileTypes[] = { { L"Punch Files", L"*.punch" } };
        std::wstring path;
        IFileOpenDialog* pFileOpen;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        if (SUCCEEDED(hr))
        {
            pFileOpen->SetFileTypes(1, fileTypes);
            hr = pFileOpen->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    path = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
            pFileOpen->Release();
        }
        CoUninitialize();
        return path;
    }
};
