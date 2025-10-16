#include <unordered_map>

#include "lib/sampgdk.h"
#include "lib/cmd.h"

#include "limits.hpp"
#include "globals.hpp"
#include "dialog.hpp"
#include "files.hpp"
#include "utils.hpp"

typedef bool (*Command)(int playerid, Cmd cmd);

class CommandRegistry {
    private:
        std::unordered_map<std::string, Command> commands;

    public:
        static CommandRegistry& Instance() {
            static CommandRegistry instance;
            return instance;
        }

        void Register(const std::string &name, Command command) {
            commands[name] = command;
        }

        void RegisterAliases(Command command, std::vector<std::string> aliases) {
            for (const auto &alias : aliases) {
                commands[alias] = command;
            }
        }

        bool Execute(int playerid, const Cmd &cmd) const {
            auto it = commands.find(cmd.name);
            if (it != commands.end()) {
                return it->second(playerid, cmd);
            }
            return false;
        }
};

#define CMD(name) \
    bool cmd_##name(int playerid, Cmd cmd); \
    struct AutoRegister_##name { \
        AutoRegister_##name() { \
            CommandRegistry::Instance().Register(#name, cmd_##name); \
        } \
    }; \
    static AutoRegister_##name _autoRegister_##name; \
    bool cmd_##name(int playerid, Cmd cmd)

#define CMD_ALIAS(name, ...) \
    bool cmd_##name(int playerid, Cmd cmd); \
    struct AutoRegister_##name { \
        AutoRegister_##name() { \
            CommandRegistry::Instance().RegisterAliases(cmd_##name, { __VA_ARGS__ }); \
        } \
    }; \
    static AutoRegister_##name _autoRegister_##name; \
    bool cmd_##name(int playerid, Cmd cmd)

