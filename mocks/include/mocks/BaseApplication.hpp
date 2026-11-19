#pragma once

#include "common/Args.hpp"
#include "common/Modes.hpp"
#include "mocks/DisabledStreamerMode.hpp"
#include "controllers/twitch/LiveController.hpp"
#include "mocks/EmptyApplication.hpp"
#include "mocks/TwitchUsers.hpp"
#include "providers/bttv/BttvLiveUpdates.hpp"
#include "providers/twitch/PubSubManager.hpp"
#include "singletons/Fonts.hpp"
#include "singletons/Settings.hpp"
#include "singletons/Theme.hpp"

#include <QString>

namespace chatterino::mock {

class NoopLiveController : public ITwitchLiveController
{
public:
    void add(const std::shared_ptr<TwitchChannel> & /*newChannel*/) override
    {
    }
};

/**
 * BaseApplication intends to be a mock application with a few more sane defaults, but with less configurability
 */
class BaseApplication : public EmptyApplication
{
public:
    BaseApplication()
        : settings(Modes::instance(), this->args, this->settingsDir.path(),
                   {.isTest = true})
        , updates(this->paths_, this->settings)
        , theme(this->paths_)
        , fonts(this->settings)
    {
    }

    explicit BaseApplication(const QString &settingsData)
        : EmptyApplication(settingsData)
        , settings(Modes::instance(), this->args, this->settingsDir.path(),
                   {.isTest = true})
        , updates(this->paths_, this->settings)
        , theme(this->paths_)
        , fonts(this->settings)
    {
    }

    Updates &getUpdates() override
    {
        return this->updates;
    }

    IStreamerMode *getStreamerMode() override
    {
        return &this->streamerMode;
    }

    Theme *getThemes() override
    {
        return &this->theme;
    }

    Fonts *getFonts() override
    {
        return &this->fonts;
    }

    ITwitchUsers *getTwitchUsers() override
    {
        return &this->twitchUsers;
    }

    BttvLiveUpdates *getBttvLiveUpdates() override
    {
        return nullptr;
    }

    SeventvEventAPI *getSeventvEventAPI() override
    {
        return nullptr;
    }

    // TwitchChannel subscribes to PubSub signals when it's constructed
    PubSub *getTwitchPubSub() override
    {
        return &this->basePubSub;
    }

    ITwitchLiveController *getTwitchLiveController() override
    {
        return &this->liveController;
    }

    Args args;
    Settings settings;
    Updates updates;
    DisabledStreamerMode streamerMode;
    Theme theme;
    Fonts fonts;
    TwitchUsers twitchUsers;
    PubSub basePubSub{"wss://127.0.0.1:9050"};
    NoopLiveController liveController;
};

}  // namespace chatterino::mock
