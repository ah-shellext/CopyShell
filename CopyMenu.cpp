#include "stdafx.h"
#include "CopyMenu.h"
#include "Utils.hpp"

#include <windows.h>
#include <atlconv.h>
#include <string>

#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

// Context menu popuped.
HRESULT STDMETHODCALLTYPE CCopyMenu::Initialize(
    __in_opt PCIDLIST_ABSOLUTE pidlFolder,
    __in_opt IDataObject *pdtobj,
    __in_opt HKEY hkeyProgID
) {
    this->pdtobj = pdtobj;
    this->pidlFolder = pidlFolder;
    this->selected_count = 0;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = {};
    if (pdtobj != nullptr && SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        auto hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != nullptr) {
            this->selected_count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        }
    }
    return S_OK;
}

// Naming
// N: No quote; Q: quote
// BCK: back slash; FNT: front slash; DBL: double bask slash; NME: name only
const UINT MENUID_RESTART_EXPLORER = 0;
const UINT MENUID_N_BCK = 1;
const UINT MENUID_Q_BCK = 2;
const UINT MENUID_N_FNT = 3;
const UINT MENUID_Q_FNT = 4;
const UINT MENUID_N_DBL = 5;
const UINT MENUID_Q_DBL = 6;
const UINT MENUID_N_NME = 7;
const UINT MENUID_Q_NME = 8;
const UINT MENUID_N_BCK_COMMA = 9;
const UINT MENUID_Q_BCK_COMMA = 10;
const UINT MENUID_N_FNT_COMMA = 11;
const UINT MENUID_Q_FNT_COMMA = 12;
const UINT MENUID_N_DBL_COMMA = 13;
const UINT MENUID_Q_DBL_COMMA = 14;
const UINT MENUID_N_NME_COMMA = 15;
const UINT MENUID_Q_NME_COMMA = 16;

// Represents menu action.
struct MenuAction {
    LPWSTR title;
    UINT id;
    UINT pos;
    bool splitter;
};

// Create menu item.
HRESULT STDMETHODCALLTYPE CCopyMenu::QueryContextMenu(
    __in HMENU hmenu,
    __in UINT indexMenu,
    __in UINT idCmdFirst,
    __in UINT idCmdLast,
    __in UINT uFlags
) {
    if (uFlags & CMF_VERBSONLY) {
        return S_OK;
    }
    if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }
    if (this->pidlFolder == nullptr && this->pdtobj == nullptr) {
        return S_FALSE;
    }

    // sub menu
    HMENU submenu = CreateMenu();

    // temp mii
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;
    // splitter mii
    MENUITEMINFO mii_splitter;
    mii_splitter.cbSize = sizeof(MENUITEMINFO);
    mii_splitter.fMask = MIIM_FTYPE;
    mii_splitter.fType = MFT_SEPARATOR;

    // menu item count
    int cmd_count;

    // menu actions
    std::vector<MenuAction> actions;
    if (this->selected_count <= 1) { // directory background or single file
        actions = {
            {_T("引用符なし バックスラッシュ (\\)"), MENUID_N_BCK, 0}, // |\|
            {_T("引用符付き バックスラッシュ (\"\\\")"), MENUID_Q_BCK, 1}, // |"\"|
            {_T("引用符なし フロントスラッシュ (/)"), MENUID_N_FNT, 2}, // |/|
            {_T("引用符付き フロントスラッシュ (\"/\")"), MENUID_Q_FNT, 3}, // |"/"|
            {_T("引用符なし ダブルバックスラッシュ (\\\\)"), MENUID_N_DBL, 4}, // |\\|
            {_T("引用符付き ダブルバックスラッシュ (\"\\\\\")"), MENUID_Q_DBL, 5}, // |"\\"|
            {_T(""), 0, 6}, // splitter
            {_T("引用符なし 名前のみ (x)"), MENUID_N_NME, 7}, // |x|
            {_T("引用符付き 名前のみ (\"x\")"), MENUID_Q_NME, 8}, // |"x"|
        };
        cmd_count = 8;
    } else { // multiple files
        actions = {
            {_T("引用符なし バックスラッシュ セミコロン (\\;\\)"), MENUID_N_BCK, 0}, // |\|
            {_T("引用符付き バックスラッシュ セミコロン (\"\\\";\"\\\")"), MENUID_Q_BCK, 1}, // |"\"|
            {_T("引用符なし フロントスラッシュ セミコロン (/;/)"), MENUID_N_FNT, 2}, // |/|
            {_T("引用符付き フロントスラッシュ セミコロン (\"/\";\"/\")"), MENUID_Q_FNT, 3}, // |"/"|
            {_T("引用符なし ダブルバックスラッシュ セミコロン (\\\\;\\\\)"), MENUID_N_DBL, 4}, // |\\|
            {_T("引用符付き ダブルバックスラッシュ セミコロン (\"\\\\\";\"\\\\\")"), MENUID_Q_DBL, 5}, // |"\\"|
            {_T(""), 0, 6}, // splitter
            {_T("引用符なし 名前のみ セミコロン (;)"), MENUID_N_NME, 7}, // |x|
            {_T("引用符付き 名前のみ セミコロン (;)"), MENUID_Q_NME, 8}, // |"x"|
            {_T(""), 0, 9}, // splitter
            {_T("引用符なし バックスラッシュ コンマ (\\,\\)"), MENUID_N_BCK_COMMA, 10},  // |\|
            {_T("引用符付き バックスラッシュ コンマ (\"\\\",\"\\\")"), MENUID_Q_BCK_COMMA, 11}, // |"\"|
            {_T("引用符なし フロントスラッシュ コンマ (/,/)"), MENUID_N_FNT_COMMA, 12}, // |/|
            {_T("引用符付き フロントスラッシュ コンマ (\"/\",\"/\")"), MENUID_Q_FNT_COMMA, 13}, // |"/"|
            {_T("引用符なし ダブルバックスラッシュ コンマ (\\\\,\\\\)"), MENUID_N_DBL_COMMA, 14}, // |\\|
            {_T("引用符付き ダブルバックスラッシュ コンマ (\"\\\\\",\"\\\\\")"), MENUID_Q_DBL_COMMA, 15}, // |"\\"|
            {_T(""), 0, 16}, // splitter
            {_T("引用符なし 名前のみ コンマ (,)"), MENUID_N_NME_COMMA,17}, // |x|
            {_T("引用符付き 名前のみ コンマ (,)"), MENUID_Q_NME_COMMA, 18}, // |"x"|
        };
        cmd_count = 16 + 1;
    }
    for (auto action : actions) {
        if (action.id == 0 && action.pos != 0) {
            InsertMenuItem(submenu, action.pos, true, &mii_splitter);
        } else {
            mii.dwTypeData = action.title;
            mii.wID = idCmdFirst + action.id;
            InsertMenuItem(submenu, action.pos, true, &mii);
        }
    }

    // out layer menu
    MENUITEMINFO mii_out;
    mii_out.cbSize = sizeof(MENUITEMINFO);
    mii_out.fMask = MIIM_STRING | MIIM_SUBMENU;
    mii_out.dwTypeData = _T("パスのコピー(&P)");
    mii_out.hSubMenu = submenu;

    // restart explorer
    MENUITEMINFO mii_re;
    mii_re.cbSize = sizeof(MENUITEMINFO);
    mii_re.fMask = MIIM_STRING | MIIM_ID;
    mii_re.dwTypeData = _T("エクスプローラーを再起動(&R)");
    mii_re.wID = idCmdFirst + MENUID_RESTART_EXPLORER;
    cmd_count++;

    InsertMenuItem(hmenu, indexMenu + 0, true, &mii_splitter);
    InsertMenuItem(hmenu, indexMenu + 1, true, &mii_out);
    InsertMenuItem(hmenu, indexMenu + 2, true, &mii_re);
    InsertMenuItem(hmenu, indexMenu + 3, true, &mii_splitter);
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (USHORT) cmd_count + 2);
}

// Invoke menu item command.
HRESULT STDMETHODCALLTYPE CCopyMenu::InvokeCommand(
    __in CMINVOKECOMMANDINFO *pici
) {
    UINT idCmd = LOWORD(pici->lpVerb);
    if (HIWORD(pici->lpVerb) != 0) {
        return E_INVALIDARG;
    }
    if (this->pidlFolder == nullptr && this->pdtobj == nullptr) {
        return S_FALSE;
    }
    if (idCmd == MENUID_RESTART_EXPLORER) {
        system("taskkill /f /im explorer.exe && start explorer.exe");
        return S_OK;
    }

    // get selected filename
    std::vector<std::wstring> sel_names;
    if (this->pidlFolder != nullptr) { // directory background
        auto current_path = new WCHAR[Utils::MAX_PATH_LENGTH];
        auto ok = SHGetPathFromIDListW(this->pidlFolder, current_path);
        if (!ok) {
            MessageBox(nullptr, _T("ファイルネームの取得にエラーが発生しました。"), _T("エラー"), MB_OK);
            return S_FALSE;
        }
        sel_names.emplace_back(current_path);
    } else { // file
        bool ok;
        sel_names = Utils::get_filenames_from_data_object(this->pdtobj, &ok);
        if (!ok) {
            MessageBox(nullptr, _T("ファイルネームの取得にエラーが発生しました。"), _T("エラー"), MB_OK);
            return S_FALSE;
        }
    }

    // menu item action
    std::wstring out;
    switch (idCmd) {
    case MENUID_N_BCK:
        out = Utils::join_filenames(sel_names, false, false, false);
        break;
    case MENUID_Q_BCK:
        out = Utils::join_filenames(sel_names, true, false, false);
        break;
    case MENUID_N_FNT:
        out = Utils::join_filenames(sel_names, false, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_Q_FNT:
        out = Utils::join_filenames(sel_names, true, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_N_DBL:
        out = Utils::join_filenames(sel_names, false, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_Q_DBL:
        out = Utils::join_filenames(sel_names, true, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_N_NME:
        out = Utils::join_filenames(sel_names, false, true, false);
        break;
    case MENUID_Q_NME:
        out = Utils::join_filenames(sel_names, true, true, false);
        break;
    case MENUID_N_BCK_COMMA:
        out = Utils::join_filenames(sel_names, false, false, true);
        break;
    case MENUID_Q_BCK_COMMA:
        out = Utils::join_filenames(sel_names, true, false, true);
        break;
    case MENUID_N_FNT_COMMA:
        out = Utils::join_filenames(sel_names, false, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_Q_FNT_COMMA:
        out = Utils::join_filenames(sel_names, true, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_N_DBL_COMMA:
        out = Utils::join_filenames(sel_names, false, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_Q_DBL_COMMA:
        out = Utils::join_filenames(sel_names, true, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_N_NME_COMMA:
        out = Utils::join_filenames(sel_names, false, true, true);
        break;
    case MENUID_Q_NME_COMMA:
        out = Utils::join_filenames(sel_names, true, true, true);
        break;
    }

    // copy to clipboard
    try {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            HGLOBAL buf = GlobalAlloc(GHND | GMEM_SHARE, (out.length() + 1) * sizeof(WCHAR));
            if (buf == 0) {
                return S_FALSE;
            }
            auto strMem = (LPWSTR) GlobalLock(buf);
            if (strMem == 0) {
                return S_FALSE;
            }
            wcscpy(strMem, out.data());
            SetClipboardData(CF_UNICODETEXT, buf);

            CloseClipboard();
            GlobalUnlock(buf);
        }
    } catch (...) { }

    return S_OK;
}
