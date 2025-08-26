#include <gui/settime_screen/SetTimeView.hpp>

SetTimeView::SetTimeView()
{

}

void SetTimeView::setupScreen()
{
    SetTimeViewBase::setupScreen();


    buttons.setL1(Buttons::WHITE);
    buttons.setL2(Buttons::WHITE);
    buttons.setR1(Buttons::AMBER);
    buttons.setR2(Buttons::WHITE);

    setPosition(Position::HOURS);

    menuRepeat.setNumberOfItems(AppType::Alarm::Repeat::REPEAT_COUNT);
    for (uint16_t i = 0; i < AppType::Alarm::Repeat::REPEAT_COUNT; i++) {
        MenuWheelItemSelected* pS = menuRepeat.getSelectedItem(i);
        pS->config(Gui::Config::Alarm::RepeatValue[i]);

        MenuWheelItemNotSelected* pN = menuRepeat.getNotSelectedItem(i);
        pN->config(Gui::Config::Alarm::RepeatValue[i]);
    }
    menuRepeat.invalidate();

    menuEffect.setNumberOfItems(AppType::Alarm::Effect::EFFECT_COUNT);
    for (uint16_t i = 0; i < AppType::Alarm::Effect::EFFECT_COUNT; i++) {
        MenuWheelItemSelected* pS = menuEffect.getSelectedItem(i);
        pS->config(Gui::Config::Alarm::EffectValue[i]);
        MenuWheelItemNotSelected* pN = menuEffect.getNotSelectedItem(i);
        pN->config(Gui::Config::Alarm::EffectValue[i]);
    }

    menuEffect.invalidate();
}

void SetTimeView::tearDownScreen()
{
    SetTimeViewBase::tearDownScreen();
}

void SetTimeView::set(uint8_t h, uint8_t m, AppType::Alarm::Repeat repeat, AppType::Alarm::Effect effect)
{
    menuTime.setTime(h, m);
    menuRepeat.selectItem(static_cast<int16_t>(repeat));
    menuEffect.selectItem(static_cast<int16_t>(effect));
}

void SetTimeView::setPosition(Position id)
{
    switch (id) {
    case Position::HOURS:
        title.set(T_TEXT_ALARM_UC);
        setActiveName(T_TEXT_HOURS);
        menuTime.setVisible(true);
        menuTime.setActiveHours();
        menuRepeat.setVisible(false);
        menuEffect.setVisible(false);
        tick.setVisible(false);
        break;
    case Position::MINUTES:
        title.set(T_TEXT_ALARM_UC);
        setActiveName(T_TEXT_MIN_DOT);
        menuTime.setVisible(true);
        menuTime.setActiveMinutes();
        menuRepeat.setVisible(false);
        menuEffect.setVisible(false);
        tick.setVisible(false);
        tick.invalidate();
        break;
    case Position::REPEAT:
        title.set(T_TEXT_ALARM_UC);
        setActiveName(T_TEXT_REPEAT);
        menuTime.setVisible(false);
        menuRepeat.setVisible(true);
        menuEffect.setVisible(false);
        tick.setVisible(false);
        tick.invalidate();
        break;
    case Position::EFFECT:
        title.set(T_TEXT_ALARM_UC);
        setActiveName(T_TEXT_ALERT);
        menuTime.setVisible(false);
        menuRepeat.setVisible(false);
        menuEffect.setVisible(true);
        tick.setVisible(true);
        tick.invalidate();
        break;
    default:
        break;
    }

    title.invalidate();
    menuTime.invalidate();
    menuRepeat.invalidate();
    menuEffect.invalidate();
    tick.invalidate();
}

void SetTimeView::setActiveName(TypedTextId msgId)
{
    Unicode::snprintf(textNameBuffer, TEXTNAME_SIZE, "%s", touchgfx::TypedText(msgId).getText());
    textName.invalidate();
}

void SetTimeView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        if (mPosition == Position::HOURS || mPosition == Position::MINUTES) {
            menuTime.decValue();
        } else if (mPosition == Position::REPEAT) {
            menuRepeat.selectPrev();
        } else if (mPosition == Position::EFFECT) {
            menuEffect.selectPrev();
        }
    }

    if (key == Gui::Config::Button::L2) {
        if (mPosition == Position::HOURS || mPosition == Position::MINUTES) {
            menuTime.incValue();
        } else if (mPosition == Position::REPEAT) {
            menuRepeat.selectNext();
        } else if (mPosition == Position::EFFECT) {
            menuEffect.selectNext();
        }
    }

    if (key == Gui::Config::Button::R1) {
        if (mPosition == Position::EFFECT) {
            // Save & exit
            uint8_t h = 0;
            uint8_t m = 0;
            menuTime.getTime(h, m);
            presenter->save(h, m, static_cast<AppType::Alarm::Repeat>(menuRepeat.getSelectedItem()), static_cast<AppType::Alarm::Effect>(menuEffect.getSelectedItem()));
            application().gotoSavedScreenNoTransition();
        }
        else {
            mPosition = static_cast<Position>(static_cast<uint8_t>(mPosition) + 1);
            setPosition(mPosition);
        }
    }

    if (key == Gui::Config::Button::R2) {
        if (mPosition == Position::HOURS) {
            // Discard & exit
            presenter->exitScreen();
        }
        else {
            mPosition = static_cast<Position>(static_cast<uint8_t>(mPosition) - 1);
            setPosition(mPosition);
        }
    }
}