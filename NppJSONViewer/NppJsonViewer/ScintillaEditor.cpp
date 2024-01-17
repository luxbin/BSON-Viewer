#include "ScintillaEditor.h"
#include <cassert>
#include <memory>

ScintillaEditor::ScintillaEditor(const NppData &nppData)
    : m_NppData(nppData)
{
    RefreshViewHandle();
}

void ScintillaEditor::RefreshViewHandle()
{
    int which = -1;
    ::SendMessage(m_NppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));
    assert(which != -1);
    if (which != -1)
        m_hScintilla = (which == 0) ? m_NppData._scintillaMainHandle : m_NppData._scintillaSecondHandle;
}

std::string ScintillaEditor::GetJsonText()
{
    m_mapUpdateStyle.clear();

    if (!m_hScintilla)
        return std::string();

    // Adjust the selection position
    RefreshSelectionPos();

    // Get only selected text if any else select all text
    size_t asciiTextLen = m_nEndPos - m_nStartPos;
    if (asciiTextLen == 0)
    {
        asciiTextLen = ::SendMessage(m_hScintilla, SCI_GETLENGTH, 0, 0);
        ::SendMessage(m_hScintilla, SCI_SETSELECTIONSTART, 0, 0);
        ::SendMessage(m_hScintilla, SCI_SETSELECTIONEND, asciiTextLen, 0);
    }
    std::unique_ptr<CHAR[]> chSelectedText = std::make_unique<CHAR[]>(asciiTextLen + 1);
    ::SendMessage(m_hScintilla, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(chSelectedText.get()));

    // Update selection position
    RefreshSelectionPos();

    return chSelectedText.get();
}

void ScintillaEditor::SetLangAsJson() const
{
    ::SendMessage(m_NppData._nppHandle, NPPM_SETCURRENTLANGTYPE, 0, LangType::L_JSON);
}

bool ScintillaEditor::IsJsonFile() const
{
    unsigned langType = 0;
    ::SendMessage(m_NppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&langType));
    return langType == LangType::L_JSON;
}

void ScintillaEditor::ReplaceSelection(const std::string &text) const
{
    ::SendMessage(m_hScintilla, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(text.c_str()));
}

void ScintillaEditor::MakeSelection(size_t start, size_t end) const
{
    ::SendMessage(m_hScintilla, SCI_SETSEL, start, end);
}

auto ScintillaEditor::GetEOL() const -> unsigned
{
    LRESULT eolMode = ::SendMessage(m_hScintilla, SCI_GETEOLMODE, 0, 0);
    return static_cast<unsigned>(eolMode);
}

auto ScintillaEditor::GetIndent() const -> std::tuple<char, unsigned>
{
    bool     useTabs    = ::SendMessage(m_hScintilla, SCI_GETUSETABS, 0, 0);
    char     indentChar = useTabs ? '\t' : ' ';
    unsigned indentLen  = useTabs ? 1 : static_cast<unsigned>(::SendMessage(m_hScintilla, SCI_GETTABWIDTH, 0, 0));
    return std::tuple<char, unsigned>(indentChar, indentLen);
}

#include <Windows.h>

void ScintillaEditor::RefreshSelectionPos()
{
    m_nStartPos = ::SendMessage(m_hScintilla, SCI_GETSELECTIONSTART, 0, 0);
    m_nEndPos   = ::SendMessage(m_hScintilla, SCI_GETSELECTIONEND, 0, 0);

    if (m_nEndPos < m_nStartPos)
        std::swap(m_nStartPos, m_nEndPos);


}

void ScintillaEditor::RefreshStyles() 
{
    RefreshViewHandle();

    std::map<int, int>::iterator it;

    for (it = m_mapUpdateStyle.begin(); it != m_mapUpdateStyle.end(); it++)
    {
        int pos = it->first;
        int end = pos + it->second;

        ::SendMessage(m_hScintilla, SCI_STARTSTYLING, pos, 0);
        ::SendMessage(m_hScintilla, SCI_SETSTYLING, it->second + 3, 0);

        //::SendMessage(m_hScintilla, SCI_STARTSTYLING, 379, 0);
        //::SendMessage(m_hScintilla, SCI_SETSTYLING, 14 + 1, 0);

    }
}

void ScintillaEditor::UpdateStyle(int pos, int length) 
{
    m_mapUpdateStyle[pos] = length;

    //char szLog[32];
    //sprintf(szLog, "***** %d, %d", pos, length);
    //OutputDebugStringA(szLog);
    //RefreshViewHandle();    

    /*
    HDC    hdc_editor = ::GetDC(m_hScintilla);
    RECT   rc;
    HBRUSH brush;
    
    

    int start_x = (int)::SendMessage(m_hScintilla, SCI_POINTXFROMPOSITION, 0, pos);
    int start_y = (int)::SendMessage(m_hScintilla, SCI_POINTYFROMPOSITION, 0, pos);
    int end_x   = (int)::SendMessage(m_hScintilla, SCI_POINTXFROMPOSITION, 0, end);
    int end_y   = (int)::SendMessage(m_hScintilla, SCI_POINTYFROMPOSITION, 0, end);

    
    int line_height = (int)::SendMessage(m_hScintilla, SCI_TEXTHEIGHT, 0, 0);
    COLORREF colorref = RGB(50, 200, 75);

    // paint swatch /////////
    // new color
    rc.left   = start_x;
    rc.top   = start_y;

    rc.right  = end_x;
    rc.bottom = end_y + 15;

    brush     = ::CreateSolidBrush(colorref);
    //::FillRect(hdc_editor, &rc, brush);
    ::DeleteObject(brush);
    */


    //int end   = pos + length;

    //int style = ::SendMessage(m_hScintilla, SCI_GETSTYLEAT, 42, 0);

    //::SendMessage(m_hScintilla, SCI_STARTSTYLING, 42, 0);
    //::SendMessage(m_hScintilla, SCI_SETSTYLING, 79, 0);

    //::ReleaseDC(m_hScintilla, hdc_editor);

    //::RedrawWindow(m_hScintilla, NULL, NULL, RDW_INVALIDATE);

    //SCI_GETSTYLEINDEXAT(position pos) → int

    int x = 0;

}