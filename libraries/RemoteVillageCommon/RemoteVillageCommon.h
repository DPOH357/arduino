#ifndef DPOH_REMOTE_VILLAGE_COMMON_H
#define DPOH_REMOTE_VILLAGE_COMMON_H

#define PIPE_1_OUT 0x7878787878LL

enum class Command
{
    Unknown,
    Request,
    SetLamp
}

struct Data
{
    Command command;
    uint32_t data;

    Data(
        const Command command_ = Command::Unknown,
        const uint32_t data_ = 0)
        : command(command_)
        , data(data_)
    {

    }
};

#endif // DPOH_REMOTE_VILLAGE_COMMON_H
