// SPDX-FileCopyrightText: 2026 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "providers/twitch/IrcMessageHandler.hpp"
#include "providers/twitch/TwitchChannel.hpp"
#include "util/VectorMessageSink.hpp"

#include <IrcMessage>
#include <QJsonArray>
#include <QJsonObject>

#include <memory>
#include <vector>

namespace chatterino::bench {

/// Parses the raw IRC lines of a recent-messages response
inline std::vector<std::unique_ptr<Communi::IrcMessage>> parseIrcMessages(
    const QJsonObject &response)
{
    std::vector<std::unique_ptr<Communi::IrcMessage>> parsed;
    for (const auto &line : response.value("messages").toArray())
    {
        std::unique_ptr<Communi::IrcMessage> msg(
            Communi::IrcMessage::fromData(line.toString().toUtf8(), nullptr));
        if (msg)
        {
            parsed.emplace_back(std::move(msg));
        }
    }
    return parsed;
}

inline std::vector<MessagePtr> buildIrcMessages(
    const std::vector<std::unique_ptr<Communi::IrcMessage>> &parsed,
    TwitchChannel *channel)
{
    VectorMessageSink sink;
    for (const auto &msg : parsed)
    {
        IrcMessageHandler::parseMessageInto(msg.get(), sink, channel);
    }
    return std::move(sink).takeMessages();
}

}  // namespace chatterino::bench
