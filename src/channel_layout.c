#include "channel_layout.h"


#include <string.h>
#include <stdlib.h>

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define CHANNEL_LAYOUT_NAME_COUNT 36
static const char * const channel_names[CHANNEL_LAYOUT_NAME_COUNT] = {
    [0]  = "FL",        /* front left */
    [1]  = "FR",        /* front right */
    [2]  = "FC",        /* front center */
    [3]  = "LFE",       /* low frequency */
    [4]  = "BL",        /* back left */
    [5]  = "BR",        /* back right */
    [6]  = "FLC",       /* front left-of-center  */
    [7]  = "FRC",       /* front right-of-center */
    [8]  = "BC",        /* back-center */
    [9]  = "SL",        /* side left */
    [10] = "SR",        /* side right */
    [11] = "TC",        /* top center */
    [12] = "TFL",       /* top front left */
    [13] = "TFC",       /* top front center */
    [14] = "TFR",       /* top front right */
    [15] = "TBL",       /* top back left */
    [16] = "TBC",       /* top back center */
    [17] = "TBR",       /* top back right */
    [29] = "DL",        /* downmix left */
    [30] = "DR",        /* downmix right */
    [31] = "WL",        /* wide left */
    [32] = "WR",        /* wide right */
    [33] = "SDL",       /* surround direct left */
    [34] = "SDR",       /* surround direct right */
    [35] = "LFE2",      /* low frequency 2 */
};

static const char *get_channel_name(int channel_id)
{
    if (channel_id < 0 || channel_id >= CHANNEL_LAYOUT_NAME_COUNT)
        return 0;
    return channel_names[channel_id];
}

#define LAYOUT_MAP_COUNT 28
static const struct {
    const char *name;
    int         nb_channels;
    uint64_t     layout;
} channel_layout_map[] = {
    { "mono",        1,  GENESIS_CH_LAYOUT_MONO },
    { "stereo",      2,  GENESIS_CH_LAYOUT_STEREO },
    { "stereo",      2,  GENESIS_CH_LAYOUT_STEREO_DOWNMIX },
    { "2.1",         3,  GENESIS_CH_LAYOUT_2POINT1 },
    { "3.0",         3,  GENESIS_CH_LAYOUT_SURROUND },
    { "3.0(back)",   3,  GENESIS_CH_LAYOUT_2_1 },
    { "3.1",         4,  GENESIS_CH_LAYOUT_3POINT1 },
    { "4.0",         4,  GENESIS_CH_LAYOUT_4POINT0 },
    { "quad",        4,  GENESIS_CH_LAYOUT_QUAD },
    { "quad(side)",  4,  GENESIS_CH_LAYOUT_2_2 },
    { "4.1",         5,  GENESIS_CH_LAYOUT_4POINT1 },
    { "5.0",         5,  GENESIS_CH_LAYOUT_5POINT0 },
    { "5.0",         5,  GENESIS_CH_LAYOUT_5POINT0_BACK },
    { "5.1",         6,  GENESIS_CH_LAYOUT_5POINT1 },
    { "5.1",         6,  GENESIS_CH_LAYOUT_5POINT1_BACK },
    { "6.0",         6,  GENESIS_CH_LAYOUT_6POINT0 },
    { "6.0(front)",  6,  GENESIS_CH_LAYOUT_6POINT0_FRONT },
    { "hexagonal",   6,  GENESIS_CH_LAYOUT_HEXAGONAL },
    { "6.1",         7,  GENESIS_CH_LAYOUT_6POINT1 },
    { "6.1",         7,  GENESIS_CH_LAYOUT_6POINT1_BACK },
    { "6.1(front)",  7,  GENESIS_CH_LAYOUT_6POINT1_FRONT },
    { "7.0",         7,  GENESIS_CH_LAYOUT_7POINT0 },
    { "7.0(front)",  7,  GENESIS_CH_LAYOUT_7POINT0_FRONT },
    { "7.1",         8,  GENESIS_CH_LAYOUT_7POINT1 },
    { "7.1(wide)",   8,  GENESIS_CH_LAYOUT_7POINT1_WIDE },
    { "7.1(wide)",   8,  GENESIS_CH_LAYOUT_7POINT1_WIDE_BACK },
    { "octagonal",   8,  GENESIS_CH_LAYOUT_OCTAGONAL },
    { "downmix",     2,  GENESIS_CH_LAYOUT_STEREO_DOWNMIX, },
    { 0 }
};

static uint64_t get_channel_layout_single(const char *name, int name_len)
{
    int i;
    char *end;
    int64_t layout;
    unsigned name_len_u = name_len;

    for (i = 0; i < LAYOUT_MAP_COUNT - 1; i++) {
        if (strlen(channel_layout_map[i].name) == name_len_u &&
            !memcmp(channel_layout_map[i].name, name, name_len_u))
            return channel_layout_map[i].layout;
    }
    for (i = 0; i < CHANNEL_LAYOUT_NAME_COUNT; i++)
        if (channel_names[i] &&
            strlen(channel_names[i]) == name_len_u &&
            !memcmp(channel_names[i], name, name_len_u))
            return (int64_t)1 << i;
    i = strtol(name, &end, 10);
    if (end - name == name_len_u ||
        (end + 1 - name == name_len_u && *end  == 'c'))
        return genesis_get_default_channel_layout(i);
    layout = strtoll(name, &end, 0);
    if (end - name == name_len_u)
        return MAX(layout, 0);
    return 0;
}

uint64_t genesis_get_channel_layout(const char *name)
{
    const char *n, *e;
    const char *name_end = name + strlen(name);
    int64_t layout = 0, layout_single;

    for (n = name; n < name_end; n = e + 1) {
        for (e = n; e < name_end && *e != '+' && *e != '|'; e++);
        layout_single = get_channel_layout_single(n, e - n);
        if (!layout_single)
            return 0;
        layout |= layout_single;
    }
    return layout;
}

/* Count number of bits set to one in x */
static int popcount(uint32_t x)
{
    x -= (x >> 1) & 0x55555555;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += x >> 8;
    return (x + (x >> 16)) & 0x3F;
}


/* Count number of bits set to one in x */
static int popcount64(uint64_t x)
{
    return popcount(x) + popcount(x >> 32);
}

int genesis_get_channel_layout_nb_channels(uint64_t channel_layout)
{
    return popcount64(channel_layout);
}

uint64_t genesis_get_default_channel_layout(int nb_channels)
{
    switch(nb_channels) {
    case 1: return GENESIS_CH_LAYOUT_MONO;
    case 2: return GENESIS_CH_LAYOUT_STEREO;
    case 3: return GENESIS_CH_LAYOUT_SURROUND;
    case 4: return GENESIS_CH_LAYOUT_QUAD;
    case 5: return GENESIS_CH_LAYOUT_5POINT0;
    case 6: return GENESIS_CH_LAYOUT_5POINT1;
    case 7: return GENESIS_CH_LAYOUT_6POINT1;
    case 8: return GENESIS_CH_LAYOUT_7POINT1;
    default: return 0;
    }
}

int genesis_get_channel_layout_channel_index(uint64_t channel_layout,
                                        uint64_t channel)
{
    if (!(channel_layout & channel) ||
        genesis_get_channel_layout_nb_channels(channel) != 1)
        return -1;
    channel_layout &= channel - 1;
    return genesis_get_channel_layout_nb_channels(channel_layout);
}

const char *genesis_get_channel_name(uint64_t channel)
{
    int i;
    if (genesis_get_channel_layout_nb_channels(channel) != 1)
        return NULL;
    for (i = 0; i < 64; i++)
        if ((1ULL<<i) & channel)
            return get_channel_name(i);
    return NULL;
}

uint64_t genesis_channel_layout_extract_channel(uint64_t channel_layout, int index)
{
    int i;

    if (genesis_get_channel_layout_nb_channels(channel_layout) <= index)
        return 0;

    for (i = 0; i < 64; i++) {
        if ((1ULL << i) & channel_layout && !index--)
            return 1ULL << i;
    }
    return 0;
}



const char *genesis_get_channel_layout_string(uint64_t channel_layout)
{
    switch(channel_layout) {
        case GENESIS_CH_LAYOUT_MONO: return "Mono";
        case GENESIS_CH_LAYOUT_STEREO: return "Stereo";
        case GENESIS_CH_LAYOUT_2POINT1: return "2.1";
        case GENESIS_CH_LAYOUT_2_1: return "2_1";
        case GENESIS_CH_LAYOUT_SURROUND: return "Surround";
        case GENESIS_CH_LAYOUT_3POINT1: return "3.1";
        case GENESIS_CH_LAYOUT_4POINT0: return "4.0";
        case GENESIS_CH_LAYOUT_4POINT1: return "4.1";
        case GENESIS_CH_LAYOUT_2_2: return "2_2";
        case GENESIS_CH_LAYOUT_QUAD: return "Quad";
        case GENESIS_CH_LAYOUT_5POINT0: return "5.0";
        case GENESIS_CH_LAYOUT_5POINT1: return "5.1";
        case GENESIS_CH_LAYOUT_5POINT0_BACK: return "5.0 Back";
        case GENESIS_CH_LAYOUT_5POINT1_BACK: return "5.1 Back";
        case GENESIS_CH_LAYOUT_6POINT0: return "6.0";
        case GENESIS_CH_LAYOUT_6POINT0_FRONT: return "6.0 Front";
        case GENESIS_CH_LAYOUT_HEXAGONAL: return "Hexagonal";
        case GENESIS_CH_LAYOUT_6POINT1: return "6.1";
        case GENESIS_CH_LAYOUT_6POINT1_BACK: return "6.1 Back";
        case GENESIS_CH_LAYOUT_6POINT1_FRONT: return "6.1 Front";
        case GENESIS_CH_LAYOUT_7POINT0: return "7.0";
        case GENESIS_CH_LAYOUT_7POINT0_FRONT: return "7.0 Front";
        case GENESIS_CH_LAYOUT_7POINT1: return "7.1";
        case GENESIS_CH_LAYOUT_7POINT1_WIDE: return "7.1 Wide";
        case GENESIS_CH_LAYOUT_7POINT1_WIDE_BACK: return "7.1 Wide Back";
        case GENESIS_CH_LAYOUT_OCTAGONAL: return "Octagonal";
        case GENESIS_CH_LAYOUT_STEREO_DOWNMIX: return "Stereo Downmix";
    }
    return "Custom";
}
