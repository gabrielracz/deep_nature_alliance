#ifndef STORY_DATA_H
#define STORY_DATA_H
#include <string>
#include <unordered_map>
#include <vector>

#include "defines.h"
#include "text.h"

enum StoryBeat {
    INTRO = 0,
    EXPOSITION,
    PROSE,
    TOLKIEN,
    NUM_STORY_BEATS
};

//                              Text Color     Background Color     Anchor Point              Screen Position   Scroll Delay
#define FEDERATION_TRANSMISSION Colors::Amber, Colors::TranspBlack, Text::Anchor::LEFTCENTER, {-1.0, 0.0, 0.0}, 0.05f
#define NARRATION               Colors::White, Colors::TBrown

// json 2.0
static const std::unordered_map<int, std::vector<Text>> STORY = {
    {INTRO, {
             {
                "Goddamnit private\n"
                " \n"
                "I told you to be back at base\n"
                "three fucking minutes ago.\n"
                "Recharge your FTL drive and\n"
                "get out of there, that's a\n"
                "goddamn restricted airspace I'm\n"
                "sure you know.\n"
                " \n"
                "give it up kid... they're gone.\n",
                FEDERATION_TRANSMISSION
             }
    }},
    {EXPOSITION,{
            {
                "After a millenia of peace and prosperity,\n"
                "the federation grew impatient with the\n"
                "stagnation of science and human advancements.\n"
            },
            {
                "They turned from the exploration of the heavens\n"
                "to the middling human genome, rife with errors...\n"
                "and potential..."
            }
    }},
    {TOLKIEN, {
            {
                "All that is gold does not glitter,\n"
                "Not all those who wander are lost;\n"
                "The old that is strong does not wither,\n"
                "Deep roots are not reached by the frost.\n"
                "From the ashes a fire shall be woken,\n"
                "A light from the shadows shall spring;\n"
                "Renewed shall be blade that was broken,\n"
                "The crownless again shall be king.\n"
            },
            {
                "The Black Rider flung back his hood,\n"
                "and behold! he had a kingly crown;\n"
                "and yet upon no head visible was it set.\n"
                "The red fires shone between it and the\n"
                "mantled shoulders vast and dark.\n"
                "From a mouth unseen\n"
                "there came a deadly laughter.\n",
            },
            {
                "The Road goes ever on and on\n"
                "Down from the door where it began.\n"
                "Now far ahead the Road has gone,\n"
                "And I must follow, if I can,\n"
                "Pursuing it with eager feet,\n"
                "Until it joins some larger way\n"
                "Where many paths and errands meet.\n"
                "And whither then? I cannot say\n",
            },
            {
                "I sit beside the fire and think\n"
                "Of all that I have seen\n"
                "Of meadow flowers and butterflies\n"
                "In summers that have been\n"

                "Of yellow leaves and gossamer\n"
                "In autumns that there were\n"
                "With morning mist and silver sun\n"
                "And wind upon my hair\n"

                "I sit beside the fire and think\n"
                "Of how the world will be\n"
                "When winter comes without a spring\n"
                "That I shall ever see\n"

                "For still there are so many things\n"
                "That I have never seen\n"
                "In every wood in every spring\n"
                "There is a different green\n"

                "I sit beside the fire and think\n"
                "Of people long ago\n"
                "And people that will see a world\n"
                "That I shall never know\n"

                "But all the while I sit and think\n"
                "Of times there were before\n"
                "I listen for returning feet\n"
                "And voices at the door\n",
            }
    }}
};
#endif