#include "TextEdit.h"

TextEdit::TextEdit(OSDTask *task, lv_obj_t *parent) : Control(task, parent)
{
    // Set up the sort of container control which has the scroll bars
    parent_obj = lv_obj_create(parent);
    lv_obj_add_style(parent_obj, ThemeManager::GetStyle(StyleAttribute::Scrollbar), LV_PART_SCROLLBAR);
    lv_obj_add_style(parent_obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(parent_obj, ScrollEventHandler, LV_EVENT_SCROLL, this);

    // Canvas child
    obj = lv_obj_create(parent_obj);
    lv_obj_add_style(obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

TextEdit::~TextEdit()
{
    if (canvas != NULL)
        delete canvas;
}

void TextEdit::SetText(std::string text)
{
    auto ll = splitString(text, '\n');
    code.reserve(ll.size());
    std::copy(std::begin(ll), std::end(ll), std::back_inserter(code));
    CalculateLongestLine();
}

std::string TextEdit::GetText()
{
    std::string c;
    for (auto &l : code)
    {
        c += l + "\n";
    }
    return c;
}

void TextEdit::Resized()
{
    SetupCanvas();
}

void TextEdit::ScrollEventHandler(lv_event_t *e)
{
    lv_obj_t *scroll = lv_event_get_target(e);
    auto scroll_y = lv_obj_get_scroll_top(scroll);
    auto scroll_x = lv_obj_get_scroll_left(scroll);
    if (scroll_x < 0)
        scroll_x = 0;
    if (scroll_y < 0)
        scroll_y = 0;

    // Work out X and Y
    auto editor = (TextEdit *)e->user_data;
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    editor->x = scroll_x / size / 2;
    editor->y = scroll_y / size;
    editor->screen_x = scroll_x / size / 2;
    editor->screen_y = scroll_y / size;

    // Render
    editor->Render();
}

void TextEdit::SetupCanvas()
{
    lv_area_t sz;
    lv_obj_get_coords(parent_obj, &sz);
    //    CLogger::Get()->Write("File Manager", LogNotice, "%d %d %d %d", sz.x1, sz.x2, sz.y1, sz.y2);

    // Resize/redraw canvas
    delete canvas;
    canvas_w = sz.x2 - sz.x1 - 12;
    canvas_h = sz.y2 - sz.y1 - 12;
    canvas = new Canvas(task, parent_obj, canvas_w, canvas_h);

    // Reset canvas styling
    canvas->SetBG(0xFFFFFF);
    canvas->SetFG(0x0);
    lv_obj_add_flag(canvas->GetFirstBuffer(), LV_OBJ_FLAG_FLOATING);
    lv_obj_align(canvas->GetFirstBuffer(), LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(canvas->GetFirstBuffer(), ThemeManager::GetStyle(StyleAttribute::WindowContent), LV_STATE_DEFAULT);

    // Render
    Render();

    // CLogger::Get()->Write("File Manager", LogNotice, "Resize %d %d/%d %d", canvas_w, canvas_h);
}

void TextEdit::CalculateLongestLine()
{
    longest_line = 0;
    for (auto &c : code)
    {
        longest_line = std::max(longest_line, c.length());
    }
}

void TextEdit::Render()
{
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    lv_obj_set_size(obj, longest_line * size + 4, code.size() * size + 4);

    // Clear
    canvas->Clear();

    // Adjust cursor etc.
    if (y < 0)
        y = 0;
    if (y >= static_cast<int>(code.size()))
        y = static_cast<int>(code.size()) - 1;
    if (x < 0)
        x = 0;
    if (x >= static_cast<int>(code[y].size()))
        x = static_cast<int>(code[y].size()) - 1;
    bool adjust = false;
    do
    {
        adjust = false;
        int bottom_y = screen_y + (canvas_h / size) - 1;
        if (y < screen_y)
        {
            //					CLogger::Get()->Write("Editor", LogNotice, "Y<: %d %d", y, screen_y);
            screen_y--;
            // screen_y -= (canvas_h/size);
            adjust = true;
        }
        if (y > bottom_y)
        {
            //					CLogger::Get()->Write("Editor", LogNotice, "Y>: %d %d", y, bottom_y);
            screen_y++;
            // screen_y += (canvas_h/size);
            adjust = true;
        }
    } while (adjust);

    // Cursor
    int64_t ex = (x - screen_x) * (size / 2);
    int64_t ey = (y - screen_y) * size;
    canvas->SetFG(0xA0A0A0);
    canvas->SetBG(0xD0D0D0);
    canvas->DrawRectangleFilled(ex, ey, ex + size / 2, ey + size, 1);
    canvas->SetBG(0xFFFFFF);
    canvas->SetFG(0x0);

    // Text
    int actual_y = 0;
    for (size_t i = screen_y; i < code.size(); i++)
    {

        // Get line
        auto line = code[i];

        int actual_x = 0;
        for (size_t j = screen_x; j < line.length(); j++)
        {
            char c = line[j];

            // Draw
            if (c == 8)
            {
                canvas->DrawMonoText(actual_x, actual_y, std::string(4, c));
            }
            else
            {
                canvas->DrawMonoText(actual_x, actual_y, std::string(1, c));
            }

            // Next character
            actual_x += size / 2;
            if (actual_x > canvas_w)
                break;
        }

        // Next line
        actual_y += size;
        if (actual_y > canvas_h)
            break;
    }
}

void TextEdit::HandleKey(Key k)
{
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    if (k.ascii == 0)
    {
        switch (k.keycode)
        {
        case KeyInsert:
            if (mode == Mode::Overwrite)
                mode = Mode::Insert;
            else
                mode = Mode::Overwrite;
        case KeyUp:
            this->y--;
            break;
        case KeyDown:
            this->y++;
            break;
        case KeyLeft:
            this->x--;
            break;
        case KeyRight:
            this->x++;
            break;
        case KeyPageUp:
            this->y -= canvas_h / size;
            break;
        case KeyPageDown:
            this->y += canvas_h / size;
            break;
        default:
            CLogger::Get()->Write("Editor", LogNotice, "Key: %d %x", k.ascii, k.keycode);
        }
    }
    else
    {
        if (k.ascii == 10)
        {
            auto l1 = code[y].substr(0, x);
            auto l2 = code[y].substr(x);
            code[y] = l1;
            code.insert(code.begin() + y + 1, l2);
            y++;
            x = 0;
            // Break line at this point
        }
        else
        {
            CLogger::Get()->Write("Editor", LogNotice, "%d", k.ascii);
            // normal character
            if (mode == Mode::Overwrite)
            {
                code[y][x] = k.ascii;
                x++;
            }
            else
            {
                code[y].insert(x, std::string(1, k.ascii));
                CalculateLongestLine();
            }
        }
    }
    Render();
}
