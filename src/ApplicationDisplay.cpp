#include "Application.h"

void Application::render(uint32_t now) {
    display_.clearFrame();
    if (isEditing()) {
        display_.text(0, 0, screen_ == Screen::Code ? F("Set Plant Code:") : F("Set Time (minutes):"));
        display_.text(1, 0, input_.text());
        if (invalid_) display_.text(2, 0, F("Enter a valid value"));
        display_.text(3, 0, F("A:OK B:Back *:Erase"));
    } else switch (screen_) {
    case Screen::Main:
        display_.text(0, 0, F("--Toftlund Airsoft--"));
        display_.text(1, 0, F("1. Select Gamemode"));
        display_.text(3, 0, F("V 2.0.0"));
        break;
    case Screen::Games:
        display_.text(0, 0, F("Game Menu:"));
        display_.text(1, 0, F("1. Domination"));
        display_.text(2, 0, F("2. Defuse the Bomb"));
        display_.text(3, 0, F("B:Back"));
        break;
    case Screen::DominationSetup:
    case Screen::DefuseSetup: {
        const bool dom = screen_ == Screen::DominationSetup;
        display_.text(0, 0, dom ? F("Domination Mode") : F("Defuse Mode"));
        display_.text(1, 0, F("1:Start 2:Time"));
        display_.text(2, 0, F("Time:"));
        display_.number(2, 5, dom ? settings_.dominationMinutes : settings_.defuseMinutes);
        display_.text(2, 9, F("min"));
        if (invalid_) display_.text(3, 0, F("Set time: 1-999 min"));
        else if (dom) display_.text(3, 0, F("B:Back"));
        else {
            display_.text(3, 0, F("3:Code "));
            display_.text(3, 7, settings_.plantCode);
            display_.text(3, 14, F("B:Back"));
        }
        break;
    }
    case Screen::Domination:
        if (domination_.running()) {
            display_.progress(0, domination_.progress());
            display_.text(1, 0, F("Time: "));
            display_.time(1, 6, Timing::seconds(domination_.remaining(now)));
            display_.text(2, 0, domination_.owner() == Team::Blue ? F("Blue Team Has Bomb") :
                               domination_.owner() == Team::Red ? F("Red Team Has Bomb") : F("Bomb Is Neutralized"));
            display_.text(3, 0, F("Red + B:Abort"));
        } else {
            display_.text(0, 0, aborted_ ? F("Game Aborted") : F("Game Over!"));
            display_.text(1, 0, F("Blue: "));
            display_.time(1, 6, domination_.blueTime() / 1000);
            display_.text(2, 0, F("Red:  "));
            display_.time(2, 6, domination_.redTime() / 1000);
            display_.text(3, 0, F("B:Exit"));
        }
        break;
    case Screen::Defuse:
        switch (defuse_.state()) {
        case DefuseGameController::State::Planting:
            display_.text(0, 0, F("Plant the Bomb!"));
            display_.text(1, 0, wrongCode_ ? F("Wrong code. Retry:") : F("Enter the code:"));
            display_.text(2, 0, input_.text());
            display_.text(3, 0, F("#:Plant B:Back *:Del"));
            break;
        case DefuseGameController::State::Armed:
            display_.text(0, 0, F("BOMB IS ARMED!"));
            display_.text(1, 0, F("Time: "));
            display_.time(1, 6, Timing::seconds(defuse_.remaining(now)));
            display_.text(2, 0, F("Hold red + blue"));
            display_.progress(3, defuse_.progress());
            break;
        case DefuseGameController::State::Defused:
            display_.text(0, 0, F("BOMB DEFUSED!"));
            display_.text(1, 0, F("CT Wins!"));
            display_.text(2, 0, F("Elapsed: "));
            display_.time(2, 9, defuse_.defusedAfter() / 1000);
            display_.text(3, 0, F("B:Exit"));
            break;
        case DefuseGameController::State::Exploded:
            display_.text(0, 0, F("BOMB HAS EXPLODED!"));
            display_.text(1, 0, F("Terrorists Win!"));
            display_.text(3, 0, F("B:Exit"));
            break;
        case DefuseGameController::State::Aborted:
            display_.text(0, 0, F("Game Aborted"));
            display_.text(3, 0, F("B:Exit"));
            break;
        }
        break;
    default: break;
    }
    display_.flush();
    dirty_ = false;
    lastRender_ = now;
}
