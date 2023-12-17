#ifndef STORY_DATA_H
#define STORY_DATA_H
#include <string>
#include <unordered_map>
#include <vector>

#include "defines.h"
#include "text.h"

enum StoryBeat {
    BEGINNING,
    INTRO,
    FOREST_THOUGHTS,
    FIRST_TREE,
    CRASH_AREA,
    INVESTIGATE_SHIP,
    DESERT_LANDING,
    DESERT_FIRST_RECORDING,
    DESERT_SECOND_RECORDING,
    DESERT_THIRD_RECORDING,
    DESERT_FOURTH_RECORDING,
    PILL,
    PICTURE,
    GOOD_END,
    CREDITS,
    BAD_END,
    TOLKIEN,
    NUM_STORY_BEATS
};

//                              Text Color     Background Color     Anchor Point              Screen Position   Scroll Delay   Font Size (default is 13.125)
#define FEDERATION_TRANSMISSION Colors::Amber, Colors::TranspBlack, Text::Anchor::LEFTCENTER, {-1.0, 0.0, 0.0}, 0.05f
#define NARRATION               Colors::White, Colors::TBrown
#define INTRO_STORY             Colors::White,   Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.04f
#define INTRO_LOADING           Colors::SlimeGreen, Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.02f
#define SHIP_ERROR              Colors::Red, Colors::TranspBlack, Text::Anchor::LEFTCENTER, {-1.0, 0.0, 0.0}, 0.05f
#define THOUGHTS                Colors::SeaBlue, Colors::TranspBlack, Text::Anchor::RIGHTCENTER, {1.0, 0.0, 0.0}, 0.05f
#define FOREST_INVESTIGATE      Colors::SeaGreen, Colors::DarkTranspBlack, Text::Anchor::TOPCENTER, {0.0, 0.7, 0.0}, 0.05f
#define ITEM                    Colors::SlimeGreen, Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.04f
#define NOTE                    Colors::White,   Colors::TBrown, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.04f
#define BLOOD_NOTE              Colors::Red,   Colors::TBrown, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.04f
#define SAD                     Colors::Red,   Colors::DarkTranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.6f
#define CHOICE                  Colors::Red,   Colors::DarkTranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.3f
#define PLAYER_ACTION           Colors::Yellow, Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.04f
#define MAJOR_TRANSMISSION      Colors::Amber, Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.05f
#define MAJOR_THOUGHTS          Colors::SeaBlue, Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.07f
#define CREDITS_TEXT            Colors::White,   Colors::TranspBlack, Text::Anchor::CENTER, {0.0, 0.0, 0.0}, 0.03f

// json 2.0
static const std::unordered_map<int, std::vector<Text>> STORY = {
    { BEGINNING, {
        {
            "Validation begin\n"
            "Printing entry01.sz [Sub Zero Cold Storage Format] ...\n"
            "# Last access [12 seconds ago] #\n"
            "[BUFFER LOADED]\n"
            "\n"
            "Press [R] to print and validate entry.",
            INTRO_LOADING
        },
        {
            "--- EXCRYPT PLAIN BLOCK START ---\n"
            "YEAR 3620 Potentia Era [1] Kinva Domin S-65\n"
            " \n"
            "Does a dream justify its costs?\n"
            " \n"
            "I am a Colonel of the DNA J8-027th Division Fifth Reich.\n"
            "After a regular reconnaissance and suppression mission\n"
            "I have been promoted to Colonel.\n"
            "This comes off of the back of many of my subordinates committing to a full\n"
            "augment after they gave their organic conscious to suppress many\n"
            "FOD stricken citizens across the J Dominion.\n"
            " \n"
            "ERROR: BUFFER FULL ([R] clears)\n",
            INTRO_STORY
        },
        {
            "After our reconnaissance this resulted in the obliteration of CIV SECTORS:\n"
            "J-98, J-67, J-04 (and the crippling of much of the rest of the sector).\n"
            "I earn my rank to little physical sacrifice.\n"
            " \n"
            "ERROR: BUFFER FULL ([R] clears)\n",
            INTRO_STORY
        },
        {
            "Today, marks what has been declared as the great 'Potentia' Era.\n"
            "After the successful testing of the MK in the unpopulated S9-6T sector.\n"
            "which has been described to the rest of the middle supercluster\n"
            "as essential resource generating technology.\n"
            " \n"
            "Us in the Inner DNA ranks know its better use as a PMDW (Powerful Mass Destruction Weapon).\n"
            "This test was done to a sector adjacent to a close friend...\n"
            " \n"
            "ERROR: BUFFER FULL ([R] clears)\n",
            INTRO_STORY
        },
        {
            "One I made a promise to.\n"
            " \n"
            "I break rank to investigate; \n"
            "the specter of bloodshed looms over me. \n"
            " \n"
            "I make the jump to sector S9-7T...\n"
            "--- EXCRYPT PLAIN BLOCK END ---\n"
            " \n"
            "READ END (Press [R] to finish validation)\n",
            INTRO_STORY
        },
        {
            "JMP Drives status: [READY]\n"
            "Target [S9-7T]\n"
            " \n"
            "Press [J] to Jump\n",
            INTRO_LOADING
        }
    }},
    {INTRO, {
             {
                "SHIP: Connection to DNANet [DISCONNECTED].\n"
                " \n"
                "Systems have been disabled:\n"
                "> JMP Drive [Disabled]\n"
                "> Combat System [Disabled]\n"
                "> Boosters [Disabled]\n"
                " \n"
                "Re-establish connection effective immediately.\n",
                SHIP_ERROR
             }
    }},
    {FOREST_THOUGHTS,{
            {
                "This planet is way too close\n"
                "to the sun to support life...\n"
                "...\n"
                "They must have built an\n"
                "artificial atmosphere.\n"
                "But my readings indicate lethal\n"
                "levels of chlorine and ammonia.",
                THOUGHTS
            }
    }},
    {FIRST_TREE, {
        {
            "Something is strange about this tree...\n"
            "The leaves dance but the air is still.\n",
            THOUGHTS
        },
        {
            "The others are completely motionless.\n"
            "How could they survive in this noxious\n"
            "environment?",
            THOUGHTS
        }
    }},
    {CRASH_AREA, {
        {
            "There appears to be a corrupting energy\n"
            "radiating from the ship affecting the\n"
            "surrounding trees.",
            FOREST_INVESTIGATE
        },
        {
            "Seems the distress beacon is still on\n"
            "but the surrounding forest floor has already\n"
            "begun swallowing the hull.",
            FOREST_INVESTIGATE
        }
    }},
    {INVESTIGATE_SHIP, {
        {
            "You enter the ship.",
            FOREST_INVESTIGATE
        }
    }},
    {PICTURE,{
        {
            "You pick up the picture frame.\n",
            ITEM
        },
        {
            "Its a picture of me and my close friend dated 15 years ago.\n"
            ". . .\n",
            MAJOR_THOUGHTS
        }
    }},
    {PILL,{
            {
                "You pick up the pill.\n"
                "There is a handwritten note attached.\n"
                " \n"
                "(Press [R] to read the note)\n",
                ITEM
            },
            {
                "Enclosed is a HO4QX2 Compound Pill the only one ever produced from our testing.\n"
                "Ingesting will immediately initiate MK assimilation.\n"
                " \n"
                "Below is my last journal entry incase this pill is recovered.\n"
                "...\n",
                NOTE
            },
            {
                "The Resistance will likely fail to create a protective barrier in time\n"
                "to save this sector from the MK blast.\n"
                " \n"
                "We have 1 day.\n"
                "Some are banking on trying to avoid the blast in the hostile edge of this sector...\n"
                "Many have already chosen to incinerate themselves trying to jump out.\n"
                "...\n",
                NOTE
            },
            {
                "Our testing has produced that dusting an object with a HO4QX2-Like compound will\n"
                "protect it from MK assimilation but human testing has lead to horrifying results.\n"
                " \n"
                "This HO4QX2 compound will assimilate all atoms in its close vicinity including human DNA\n"
                "creating something (from our testing) completely random, turning rock matter\n"
                "into plant matter for example, or worse a melding of the victim and its surroundings.\n"
                "...\n",
                NOTE
            },
            {
                "Ever since joining the resistance I have developed what has been called\n"
                " \n"
                "Fear Of Death. \n"
                " \n"
                "Which is a capital punishment once diagnosed.\n"
                "...\n",
                NOTE
            },
            {
                "I found out recently that historically the human experience was\n"
                "completely unlike how it is today.\n"
                "Many many years ago much of what was expected of us now to achieve day to day\n"
                "was completely unfeasible and 'unnatural';\n"
                "interestingly Fear Of Death, was seen as a natural thing.\n"
                "A common part of the human experience.\n"
                "I have watched as great successful leaps into beneficial and powerful technology\n"
                "have happened frequently and rapidly\n"
                "Many things which have improved the lives of billions ultimately\n"
                "put more unnatural expectations on each and every one of them.\n"
                "...\n",
                NOTE
            },
            {
                "It is funny how I reach this revelation when tomorrow a bomb will strike\n"
                "and take everything natural about me scrambling it with\n"
                "every man made building and circuit board within this city.\n"
                 "...\n",
                 NOTE
            },
            {
                "Many years ago a childhood friend made a promise to me.\n"
                "A dream I think about everyday...\n"
                "One day we would reach the outer stars and form a life together...\n"
                "Only high ranking DNA officials can ever hope to reach those stars.\n"
                "Its to a DNA weapon I meet my demise...\n"
                " \n"
                "Does a dream justify its costs?\n"
                "...\n",
                NOTE
            },
            {
                "[. . .]\n",
                SAD
            },
            {
                "Does a dream justify its costs? [Y]/[N] \n",
                CHOICE 
            }

    }},
    {GOOD_END,{
            {
                "*You take the pill*\n",
                PLAYER_ACTION
            },
            {
                "I hope to become a mountain.\n"
                "Covered in trees, rocks and soil.\n"
                "May only the wind reach my peak.\n"
                "May animals feed and play in my hands.\n"
                "May I be unmovable to any force of will.\n",
                MAJOR_THOUGHTS
            }
    }},
    {CREDITS,{
            {
                "GAME MADE BY:\n"
                "Sean, Gabe and Q\n",
                CREDITS_TEXT
            }
    }},
    {BAD_END,{
            {
                "*You pocket the pill*\n",
                PLAYER_ACTION
            },
            {
                "Received transmission...\n"
                "(Press [R] to read)\n",
                FEDERATION_TRANSMISSION
            },
            {
                "Sent: [General Ferr] \n"
                " \n"
                "--- BEGIN EXCRYPT TRANSMISSION ---\n"
                "Congratulations!\n"
                " \n"
                "Great! Great success Colonel!\n"
                " \n"
                "You will make an outstanding DNA General some day!\n"
                "You are the bright shining star the people of the\n"
                "new generation desperately need.\n"
                " \n"
                "Unfortunately, your ships JMP Drive is indeed broken.\n"
                "It will be a few hours until the rescue crew arrives.\n"
                " \n"
                "How about you enjoy the scenery Colonel. Heh.\n"
                " \n"
                "You earned it soldier.\n"
                "--- END EXCRYPT TRANSMISSION ---\n",
                MAJOR_TRANSMISSION
            },
            {
                "<UPGRADE NOTICE>\n"
                "Violence Mode [Enabled]\n"
                "Press [V] to toggle.\n",
                SHIP_ERROR
            }
    }},
    {DESERT_LANDING,{
            {
                "Me so hot papi must get to the shade \n",
                THOUGHTS
            }
    }},
    {DESERT_FIRST_RECORDING,{
            {
                "Oh thats nice and cool, why is this planet like this? WHEre the ocean cro? \n",
                THOUGHTS
            }
    }},
    {DESERT_SECOND_RECORDING,{
            {
                "second \n",
                THOUGHTS
            }
    }},
    {DESERT_THIRD_RECORDING,{
            {
                "third \n",
                THOUGHTS
            }
    }},
    {DESERT_FOURTH_RECORDING,{
            {
                "fourth \n",
                THOUGHTS
            }
    }}
};
#endif