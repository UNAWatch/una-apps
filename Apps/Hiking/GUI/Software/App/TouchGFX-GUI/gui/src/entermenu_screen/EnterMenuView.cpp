#include <gui/entermenu_screen/EnterMenuView.hpp>
#include <images/BitmapDatabase.hpp>

EnterMenuView::EnterMenuView()
{

}

void EnterMenuView::setupScreen()
{
    EnterMenuViewBase::setupScreen();

    menu.setTitle(T_TEXT_APP_NAME_UC);

    menu.setNumberOfItems(Gui::Menu::ID_COUNT);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;


    // START
    pS = menu.getSelectedItem(Gui::Menu::ID_START);
    pS->config(T_TEXT_START);
    pS->setMsgTypedTextId(T_TMP_SEMIBOLD_35);

    pN = menu.getNotSelectedItem(Gui::Menu::ID_START);
    pN->config(T_TEXT_START);


    // SETTINGS
    pS = menu.getSelectedItem(Gui::Menu::ID_SETTINGS);
    pS->config(T_TEXT_SETTINGS);

    pN = menu.getNotSelectedItem(Gui::Menu::ID_SETTINGS);
    pN->config(T_TEXT_SETTINGS);

    // LAST_ACTIVITY
    pS = menu.getSelectedItem(Gui::Menu::ID_LAST_ACTIVITY);
    pS->config(T_TEXT_LAST_ACTIVITY);

    pN = menu.getNotSelectedItem(Gui::Menu::ID_LAST_ACTIVITY);
    pN->config(T_TEXT_LAST_ACTIVITY);

    updBackground();

    menu.invalidate();
}

void EnterMenuView::tearDownScreen()
{
    EnterMenuViewBase::tearDownScreen();
}


void EnterMenuView::setGpsFix(bool state)
{
    mGpsFix = state;

    if (mGpsFix) {
        menu.setInfoMsg(T_TEXT_SIGNAL_ACQUIRED);
    } else {
        menu.setInfoMsg(T_TEXT_ACQUIRING_SIGNAL);
    }

    updBackground();
}

void EnterMenuView::setSummaryAvailable(bool state)
{
    mSummaryAvailable = state;
    updBackground();
}

void EnterMenuView::setPositionId(uint16_t id)
{
    menu.selectItem(id);
}

uint16_t EnterMenuView::getPositionId()
{
    return menu.getSelectedItem();
}

void EnterMenuView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
        // Change background in the middle of animation
        mBackgroundUpdDelay = Gui::Config::kMenuAnimationSteps - 1;
    }

    if (key == Gui::Config::Button::L2) {
        // Change background in the middle of animation
        menu.selectNext();
        mBackgroundUpdDelay = Gui::Config::kMenuAnimationSteps - 1;
    }

    if (key == Gui::Config::Button::R1) {
        uint32_t id = menu.getSelectedItem();

        switch (id) {
            case Gui::Menu::ID_START:
                if (mGpsFix == true) {
                    presenter->startTrack();
                    application().gotoTrackScreenNoTransition();
                }
                break;
            case Gui::Menu::ID_SETTINGS:
                application().gotoMenuSettingsScreenNoTransition();
                break;
            case Gui::Menu::ID_LAST_ACTIVITY:
                if (mSummaryAvailable) {
                    application().gotoTrackSummaryScreenNoTransition();
                }
                break;
            default:
                break;
        };
    }

    if (key == Gui::Config::Button::R2) {
        presenter->exitApp();
    }
}

void EnterMenuView::handleTickEvent()
{
    if (mBackgroundUpdDelay > 0) {
        if (mBackgroundUpdDelay == 1) {
            updBackground();
        }
        mBackgroundUpdDelay--;
    }
}

void EnterMenuView::updBackground()
{
    uint32_t id = menu.getSelectedItem();
    if (id == Gui::Menu::ID_START) {
        if (mGpsFix == false) {
            menu.setBackground(BITMAP_BACKGROUND_GRAY_ID);
            menu.getButtons().setR1(Buttons::NONE);
        } else {
            menu.setBackground(BITMAP_BACKGROUND_TEAL_ID);
            menu.getButtons().setR1(Buttons::AMBER);
        }
    } else if (id == Gui::Menu::ID_LAST_ACTIVITY) {
        if (mSummaryAvailable == false) {
            menu.setBackground(BITMAP_BACKGROUND_GRAY_ID);
            menu.getButtons().setR1(Buttons::NONE);
        } else {
            menu.setBackground(BITMAP_BACKGROUND_TEALDARK_ID);
            menu.getButtons().setR1(Buttons::AMBER);
        }
    } else {
        menu.setBackground(BITMAP_BACKGROUND_TEALDARK_ID);
        menu.getButtons().setR1(Buttons::AMBER);
    }
}
