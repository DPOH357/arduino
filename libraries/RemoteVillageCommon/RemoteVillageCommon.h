#ifndef DPOH_REMOTE_VILLAGE_COMMON_H
#define DPOH_REMOTE_VILLAGE_COMMON_H

#define PIPE_1_OUT 0x7878787878LL

enum class RadioCommand
{
    Unknown,
    Request,
    SetLamp
};

struct RadioData
{
    RadioCommand command;
    uint32_t value;

    RadioData(
        const RadioCommand command_ = RadioCommand::Unknown,
        const uint32_t value_ = 0)
        : command(command_)
        , value(value_)
    {

    }
};

#endif // DPOH_REMOTE_VILLAGE_COMMON_H
