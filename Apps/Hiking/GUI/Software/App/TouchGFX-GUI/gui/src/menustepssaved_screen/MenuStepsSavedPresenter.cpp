#include <gui/menustepssaved_screen/MenuStepsSavedView.hpp>
#include <gui/menustepssaved_screen/MenuStepsSavedPresenter.hpp>

MenuStepsSavedPresenter::MenuStepsSavedPresenter(MenuStepsSavedView& v)
    : view(v)
{

}

void MenuStepsSavedPresenter::activate()
{
    view.setSteps(model->getSettings().alertSteps);
}

void MenuStepsSavedPresenter::deactivate()
{

}
