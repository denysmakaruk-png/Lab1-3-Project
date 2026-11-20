#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "GeometryProjectEngine.hpp"
#include "DataGenerator.hpp"



static GeometryProjectEngine g_engine(false);
static HWND g_hMainWnd = nullptr;
static HWND g_hAddCircleWnd = nullptr;
static HWND g_hAddLineWnd = nullptr;
static HWND g_hTransformWnd = nullptr;

enum ControlID {
    
    IDC_MAIN_LIST_CIRCLES   = 101, 
    IDC_MAIN_LIST_LINES     = 102, 
    IDC_MAIN_BTN_ADD_CIRCLE = 103, 
    IDC_MAIN_BTN_ADD_LINE   = 104, 
    IDC_MAIN_BTN_TRANSFORM  = 105,
    IDC_MAIN_BTN_SORT       = 106, 
    IDC_MAIN_BTN_RANDOM     = 107, 
    IDC_MAIN_BTN_SWITCH_CNT = 108,
    IDC_MAIN_BTN_SAVE_JSON  = 109,
    IDC_MAIN_BTN_LOAD_JSON  = 110, 
    IDC_MAIN_BTN_CLEAR      = 111, 
    IDC_MAIN_LBL_STATUS     = 112, 

    IDC_CIR_EDIT_X          = 201, 
    IDC_CIR_EDIT_Y          = 202,
    IDC_CIR_EDIT_R          = 203, 
    IDC_CIR_BTN_OK          = 204,
    IDC_CIR_BTN_CANCEL      = 205, 

    
    IDC_LINE_EDIT_A         = 301, 
    IDC_LINE_EDIT_B         = 302, 
    IDC_LINE_EDIT_C         = 303, 
    IDC_LINE_BTN_OK         = 304, 
    IDC_LINE_BTN_CANCEL     = 305, 

    IDC_TR_BTN_INTERSECT    = 401, 
    IDC_TR_BTN_INVERT       = 402, 
    IDC_TR_BTN_REFLECT      = 403, 
    IDC_TR_EDIT_RESULT      = 404  
};

static std::wstring toWString(const std::string& s) {
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring ws(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws[0], len);
    return ws;
}

static double getEditDouble(HWND hEdit) {
    wchar_t buf[128] = {0};
    GetWindowTextW(hEdit, buf, 127);
    return _wtof(buf);
}

static void refreshMainSceneUI() {
    if (!g_hMainWnd) return;

    HWND hListCircles = GetDlgItem(g_hMainWnd, IDC_MAIN_LIST_CIRCLES);
    HWND hListLines   = GetDlgItem(g_hMainWnd, IDC_MAIN_LIST_LINES);
    HWND hStatus      = GetDlgItem(g_hMainWnd, IDC_MAIN_LBL_STATUS);

    SendMessageW(hListCircles, LB_RESETCONTENT, 0, 0);
    auto& circles = g_engine.getCircles();
    for (size_t i = 0; i < circles.size(); ++i) {
        std::wstring item = L"[" + std::to_wstring(i) + L"] " + toWString(circles[i].toString());
        SendMessageW(hListCircles, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    }
    if (circles.size() > 0) SendMessageW(hListCircles, LB_SETCURSEL, 0, 0);

    SendMessageW(hListLines, LB_RESETCONTENT, 0, 0);
    auto& lines = g_engine.getLines();
    for (size_t i = 0; i < lines.size(); ++i) {
        std::wstring item = L"[" + std::to_wstring(i) + L"] " + toWString(lines[i].toString());
        SendMessageW(hListLines, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    }
    if (lines.size() > 0) SendMessageW(hListLines, LB_SETCURSEL, 0, 0);

    std::wstring statusText = L"Контейнер: " + toWString(g_engine.getContainerType()) +
        L" | Кіл: " + std::to_wstring(circles.size()) +
        L" | Прямих: " + std::to_wstring(lines.size());
    SetWindowTextW(hStatus, statusText.c_str());
}


LRESULT CALLBACK AddCircleWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Центр X:", WS_VISIBLE | WS_CHILD, 20, 20, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 120, 24, hwnd, (HMENU)IDC_CIR_EDIT_X, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Центр Y:", WS_VISIBLE | WS_CHILD, 20, 55, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 55, 120, 24, hwnd, (HMENU)IDC_CIR_EDIT_Y, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Радіус R:", WS_VISIBLE | WS_CHILD, 20, 90, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"4.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 90, 120, 24, hwnd, (HMENU)IDC_CIR_EDIT_R, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"Додати коло", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 135, 105, 30, hwnd, (HMENU)IDC_CIR_BTN_OK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Скасувати", WS_VISIBLE | WS_CHILD, 135, 135, 105, 30, hwnd, (HMENU)IDC_CIR_BTN_CANCEL, nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDC_CIR_BTN_OK) {
                try {
                    double x = getEditDouble(GetDlgItem(hwnd, IDC_CIR_EDIT_X));
                    double y = getEditDouble(GetDlgItem(hwnd, IDC_CIR_EDIT_Y));
                    double r = getEditDouble(GetDlgItem(hwnd, IDC_CIR_EDIT_R));
                    g_engine.addCircle(Geometry::Circle2D(Geometry::Point2D(x, y), r));
                    refreshMainSceneUI();
                    DestroyWindow(hwnd);
                } catch (const std::exception& ex) {
                    MessageBoxW(hwnd, toWString(ex.what()).c_str(), L"Помилка вводу", MB_ICONERROR);
                }
                return 0;
            }
            if (id == IDC_CIR_BTN_CANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_DESTROY:
            g_hAddCircleWnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK AddLineWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Коеф. A:", WS_VISIBLE | WS_CHILD, 20, 20, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 120, 24, hwnd, (HMENU)IDC_LINE_EDIT_A, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Коеф. B:", WS_VISIBLE | WS_CHILD, 20, 55, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"1.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 55, 120, 24, hwnd, (HMENU)IDC_LINE_EDIT_B, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Коеф. C:", WS_VISIBLE | WS_CHILD, 20, 90, 90, 22, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"-2.0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 90, 120, 24, hwnd, (HMENU)IDC_LINE_EDIT_C, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"Додати пряму", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 135, 105, 30, hwnd, (HMENU)IDC_LINE_BTN_OK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Скасувати", WS_VISIBLE | WS_CHILD, 135, 135, 105, 30, hwnd, (HMENU)IDC_LINE_BTN_CANCEL, nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDC_LINE_BTN_OK) {
                try {
                    double a = getEditDouble(GetDlgItem(hwnd, IDC_LINE_EDIT_A));
                    double b = getEditDouble(GetDlgItem(hwnd, IDC_LINE_EDIT_B));
                    double c = getEditDouble(GetDlgItem(hwnd, IDC_LINE_EDIT_C));
                    g_engine.addLine(Geometry::Line2D(a, b, c));
                    refreshMainSceneUI();
                    DestroyWindow(hwnd);
                } catch (const std::exception& ex) {
                    MessageBoxW(hwnd, toWString(ex.what()).c_str(), L"Помилка вводу", MB_ICONERROR);
                }
                return 0;
            }
            if (id == IDC_LINE_BTN_CANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_DESTROY:
            g_hAddLineWnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK TransformWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"BUTTON", L"Перетин (Коло ∩ Пряма)", WS_VISIBLE | WS_CHILD, 15, 15, 175, 30, hwnd, (HMENU)IDC_TR_BTN_INTERSECT, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Інверсія прямої в колі", WS_VISIBLE | WS_CHILD, 200, 15, 175, 30, hwnd, (HMENU)IDC_TR_BTN_INVERT, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Симетрія кола відн. прямої", WS_VISIBLE | WS_CHILD, 385, 15, 195, 30, hwnd, (HMENU)IDC_TR_BTN_REFLECT, nullptr, nullptr);

            CreateWindowW(L"EDIT", L"Оберіть коло та пряму у головному вікні, потім натисніть операцію вище...",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
                15, 60, 565, 180, hwnd, (HMENU)IDC_TR_EDIT_RESULT, nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            HWND hRes = GetDlgItem(hwnd, IDC_TR_EDIT_RESULT);
            int cIdx = static_cast<int>(SendMessageW(GetDlgItem(g_hMainWnd, IDC_MAIN_LIST_CIRCLES), LB_GETCURSEL, 0, 0));
            int lIdx = static_cast<int>(SendMessageW(GetDlgItem(g_hMainWnd, IDC_MAIN_LIST_LINES), LB_GETCURSEL, 0, 0));

            if (cIdx < 0 || lIdx < 0 || cIdx >= static_cast<int>(g_engine.getCircles().size()) || lIdx >= static_cast<int>(g_engine.getLines().size())) {
                SetWindowTextW(hRes, L"Помилка: спочатку оберіть хоча б одне коло та одну пряму у списках головного вікна.");
                return 0;
            }

            const auto& circle = g_engine.getCircles()[cIdx];
            const auto& line   = g_engine.getLines()[lIdx];

            if (id == IDC_TR_BTN_INTERSECT) {
                auto pts = circle.intersect(line);
                std::ostringstream oss;
                oss << "Коло: " << circle.toString() << "\r\nПряма: " << line.toString() << "\r\n";
                oss << "Знайдено точок перетину: " << pts.size() << "\r\n";
                for (const auto& p : pts) oss << "  Точка: " << p.toString() << "\r\n";
                if (!pts.empty()) {
                    oss << "Кут перетину: " << (circle.angleWith(line) * 180.0 / Geometry::PI) << " deg\r\n";
                }
                SetWindowTextW(hRes, toWString(oss.str()).c_str());
                return 0;
            }

            if (id == IDC_TR_BTN_INVERT) {
                auto inv = circle.invert(line);
                std::ostringstream oss;
                oss << "Інверсія прямої " << line.toString() << "\r\nвідносно кола " << circle.toString() << ":\r\n";
                if (std::holds_alternative<Geometry::Circle2D>(inv)) {
                    oss << "Результат -> Коло: " << std::get<Geometry::Circle2D>(inv).toString() << "\r\n";
                } else {
                    oss << "Результат -> Пряма: " << std::get<Geometry::Line2D>(inv).toString() << "\r\n";
                }
                SetWindowTextW(hRes, toWString(oss.str()).c_str());
                return 0;
            }

            if (id == IDC_TR_BTN_REFLECT) {
                auto refl = circle.reflectAcross(line);
                std::ostringstream oss;
                oss << "Симетричне відображення кола " << circle.toString()
                    << "\r\nвідносно прямої " << line.toString() << ":\r\n"
                    << "Образ -> " << refl.toString() << "\r\n";
                SetWindowTextW(hRes, toWString(oss.str()).c_str());
                return 0;
            }
            break;
        }
        case WM_DESTROY:
            g_hTransformWnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Кола у сцені (IList<Circle2D>):", WS_VISIBLE | WS_CHILD, 15, 10, 340, 20, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"LISTBOX", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                15, 35, 355, 220, hwnd, (HMENU)IDC_MAIN_LIST_CIRCLES, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Прямі у сцені (IList<Line2D>):", WS_VISIBLE | WS_CHILD, 390, 10, 340, 20, hwnd, nullptr, nullptr, nullptr);
            CreateWindowW(L"LISTBOX", nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                390, 35, 355, 220, hwnd, (HMENU)IDC_MAIN_LIST_LINES, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"+ Додати коло", WS_VISIBLE | WS_CHILD, 15, 270, 135, 32, hwnd, (HMENU)IDC_MAIN_BTN_ADD_CIRCLE, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"+ Додати пряму", WS_VISIBLE | WS_CHILD, 160, 270, 135, 32, hwnd, (HMENU)IDC_MAIN_BTN_ADD_LINE, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Геометрія / Інверсія", WS_VISIBLE | WS_CHILD, 305, 270, 165, 32, hwnd, (HMENU)IDC_MAIN_BTN_TRANSFORM, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Сортувати (R desc)", WS_VISIBLE | WS_CHILD, 480, 270, 145, 32, hwnd, (HMENU)IDC_MAIN_BTN_SORT, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Очистити", WS_VISIBLE | WS_CHILD, 635, 270, 110, 32, hwnd, (HMENU)IDC_MAIN_BTN_CLEAR, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"Випадкові дані", WS_VISIBLE | WS_CHILD, 15, 312, 135, 32, hwnd, (HMENU)IDC_MAIN_BTN_RANDOM, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"ArrayList <-> LinkedList", WS_VISIBLE | WS_CHILD, 160, 312, 185, 32, hwnd, (HMENU)IDC_MAIN_BTN_SWITCH_CNT, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Зберегти JSON (Лаб 3)", WS_VISIBLE | WS_CHILD, 355, 312, 185, 32, hwnd, (HMENU)IDC_MAIN_BTN_SAVE_JSON, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Завантажити JSON", WS_VISIBLE | WS_CHILD, 550, 312, 195, 32, hwnd, (HMENU)IDC_MAIN_BTN_LOAD_JSON, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Готово до роботи", WS_VISIBLE | WS_CHILD | SS_SUNKEN,
                15, 358, 730, 24, hwnd, (HMENU)IDC_MAIN_LBL_STATUS, nullptr, nullptr);

            g_engine.addCircle(Geometry::Circle2D(Geometry::Point2D(0.0, 0.0), 4.0));
            g_engine.addCircle(Geometry::Circle2D(Geometry::Point2D(3.0, 4.0), 2.5));
            g_engine.addLine(Geometry::Line2D::fromPoints(Geometry::Point2D(-5.0, 2.0), Geometry::Point2D(5.0, 2.0)));
            refreshMainSceneUI();
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDC_MAIN_BTN_ADD_CIRCLE) {
                if (!g_hAddCircleWnd) {
                    g_hAddCircleWnd = CreateWindowW(L"AddCircleWndClass", L"Нове коло (Circle2D)",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                        250, 200, 280, 220, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
                } else {
                    SetForegroundWindow(g_hAddCircleWnd);
                }
                return 0;
            }
            if (id == IDC_MAIN_BTN_ADD_LINE) {
                if (!g_hAddLineWnd) {
                    g_hAddLineWnd = CreateWindowW(L"AddLineWndClass", L"Нова пряма (Line2D)",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                        280, 230, 280, 220, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
                } else {
                    SetForegroundWindow(g_hAddLineWnd);
                }
                return 0;
            }
            if (id == IDC_MAIN_BTN_TRANSFORM) {
                if (!g_hTransformWnd) {
                    g_hTransformWnd = CreateWindowW(L"TransformWndClass", L"Аналізатор перетинів, симетрії та інверсії",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                        220, 260, 610, 300, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
                } else {
                    SetForegroundWindow(g_hTransformWnd);
                }
                return 0;
            }
            if (id == IDC_MAIN_BTN_SORT) {
                g_engine.sortCirclesByRadiusDesc();
                refreshMainSceneUI();
                return 0;
            }
            if (id == IDC_MAIN_BTN_RANDOM) {
                static unsigned int seed = 100;
                DataGenerator gen(seed++);
                gen.fillRandomCircles(g_engine.getCircles(), 2);
                gen.fillRandomLines(g_engine.getLines(), 1);
                refreshMainSceneUI();
                return 0;
            }
            if (id == IDC_MAIN_BTN_SWITCH_CNT) {
                bool toLinked = (g_engine.getContainerType() == "ArrayList");
                g_engine.switchContainerImplementation(toLinked);
                refreshMainSceneUI();
                return 0;
            }
            if (id == IDC_MAIN_BTN_SAVE_JSON) {
                try {
                    g_engine.exportJson("gui_scene.json");
                    MessageBoxW(hwnd, L"Сцену збережено у файл gui_scene.json (nlohmann/json).", L"Експорт JSON", MB_ICONINFORMATION);
                } catch (const std::exception& ex) {
                    MessageBoxW(hwnd, toWString(ex.what()).c_str(), L"Помилка", MB_ICONERROR);
                }
                return 0;
            }
            if (id == IDC_MAIN_BTN_LOAD_JSON) {
                try {
                    g_engine.importJson("gui_scene.json");
                    refreshMainSceneUI();
                    MessageBoxW(hwnd, L"Сцену успішно завантажено з gui_scene.json.", L"Імпорт JSON", MB_ICONINFORMATION);
                } catch (const std::exception& ex) {
                    MessageBoxW(hwnd, toWString(ex.what()).c_str(), L"Помилка", MB_ICONERROR);
                }
                return 0;
            }
            if (id == IDC_MAIN_BTN_CLEAR) {
                g_engine.getCircles().clear();
                g_engine.getLines().clear();
                refreshMainSceneUI();
                return 0;
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    WNDCLASSW wcMain = {};
    wcMain.lpfnWndProc   = MainWndProc;
    wcMain.hInstance     = hInstance;
    wcMain.lpszClassName = L"MainSceneWndClass";
    wcMain.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcMain.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wcMain);

    WNDCLASSW wcCircle = wcMain;
    wcCircle.lpfnWndProc   = AddCircleWndProc;
    wcCircle.lpszClassName = L"AddCircleWndClass";
    RegisterClassW(&wcCircle);

    WNDCLASSW wcLine = wcMain;
    wcLine.lpfnWndProc   = AddLineWndProc;
    wcLine.lpszClassName = L"AddLineWndClass";
    RegisterClassW(&wcLine);

    WNDCLASSW wcTr = wcMain;
    wcTr.lpfnWndProc   = TransformWndProc;
    wcTr.lpszClassName = L"TransformWndClass";
    RegisterClassW(&wcTr);

    g_hMainWnd = CreateWindowW(L"MainSceneWndClass",
        L"Лаб 2 GUI + Проєкт: Аналізатор 2D-геометрії та структур даних",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        150, 120, 775, 435, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
