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
    SHIP_COMMS,
    SHIP_SYSTEMS_ACTIVATED,
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
#define SHIP_ACTIVATED          Colors::SeaGreen, Colors::TranspBlack, Text::Anchor::LEFTCENTER, {-1.0, 0.0, 0.0}, 0.05f
#define THOUGHTS                Colors::SeaBlue, Colors::TranspBlack, Text::Anchor::RIGHTCENTER, {1.0, 0.0, 0.0}, 0.1f
#define FOREST_INVESTIGATE      Colors::SeaGreen, Colors::DarkTranspBlack, Text::Anchor::TOPCENTER, {0.0, 0.7, 0.0}, 0.03f
#define COMMS_RECORD            Colors::Amber, Colors::TranspBlack, Text::Anchor::TOPCENTER, {0.0, 0.4, 0.0}, 0.06f
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
                "They must have built an\n"
                "artificial atmosphere.\n"
                " \n"
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
            "I know this jet,\n" 
            "They used to fly these in the early days of the alliance.",
            FOREST_INVESTIGATE
        },
        {
            "There appears to be strong nuclear decay\n"
            "radiating from the ship affecting the\n"
            "surrounding trees.",
            FOREST_INVESTIGATE
        },
        {
            "The distress beacon is still active...\n"
            "but the forest floor has already\n"
            "begun swallowing the hull.\n",
            FOREST_INVESTIGATE
        }
    }},
    {INVESTIGATE_SHIP, {
        {
            "You open the canopy and find the cockpit in ruins.\n"
            "A nest of shattered controls and twisted metal.\n"
            " \n"
            "The space is lined with irradiated organic matter\n"
            "emitting enough gamma rays to rip apart an\n"
            "unprotected soldier in minutes...\n",
            FOREST_INVESTIGATE
        },
        {
            "Dim lights flicker off of every surface.\n"
            "A large red indicator pulses behind the yolk.\n",
            FOREST_INVESTIGATE
        },
        {
            "The ship's avionics unit...\n"
            "You offload the data and find a flight record.",
            FOREST_INVESTIGATE
        },
        {
            "[14156] ALERT           - spacecraft not authorized for takeoff\n"
            "[14779] SYSTEM OVERRIDE - spacecraft systems engaged\n"
            "[15139] LAUNCH SEQUENCE - decoupling from station XEON\n"
            "[16149] JUMP SEQUENCE   - begin jump sequence to sector UNKNOWN\n"
            "[16993] WARNING         - gamma ray surge detected\n"
            "[16994] WARNING         - neutron overload detected\n"
            "[16995] WARNING         - alpha particle overload detected\n"
            "[17081] ERROR           - control systems SHUTDOWN\n"
            "[17062] ERROR           - communications SHUTDOWN\n"
            "[17555] ERROR           - MAYDAY transmission fail\n"
            "[18901] ALERT           - begin emergency landing procedure\n"
            "[19991] WARNING         - cabin pressure CRITICAL\n"
            "[19992] WARNING         - temperature CRITICAL\n"
            "[20031] ERROR           - pilot ejection fail\n"
            "[24666] WARNING         - crash detected. enabling distress beacon\n",
            COMMS_RECORD
        },
        {
            "The record is dated just 4 days ago.\n"
            "The system must have been corrupted in the crash...\n",
            FOREST_INVESTIGATE
        },
        {
            "You notice a makeshift device inserted into the computer\n"
            "You remove it and the ship goes dark...\n"
            " \n"
            "This might be useful.",
            FOREST_INVESTIGATE
        },
    }},
    {SHIP_SYSTEMS_ACTIVATED, {
        {
            "You insert the device into the flight computer\n",
            FOREST_INVESTIGATE
        },
        {
            "> Combat System [ENABLED]\n"
            "> Boosters      [ENABLED]\n",
            SHIP_ACTIVATED
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
                "As the scorching desert sun beats down on me, I can't shake the disorientation from the crash.\n"
                "My ship, my lifeline, is nowhere to be seen.\n"
                "A distant tower emerges through the haze, a beacon of hope amid the relentless sun.\n"
                "Must find shade, must reach that tower.\n",
                THOUGHTS
            }
    }},
    {DESERT_FIRST_RECORDING,{
            {
                "Ahh thats better.\n"
                "I didn't think I was gonna make it.\n"
                "Wait a second. What planet is this. It is __planet name__!\n"
                "Was my data wrong? From my memory this was an ocean planet, entirely covered in water.\n"
                "The landscape unfolds with more towers stretching into the distance\n"
                "Who constructed these structures, and what purpose do they serve in an entirely aqueous world?\n"
                "Only one way to find out...\n",
                THOUGHTS
            }
    }},
    {DESERT_SECOND_RECORDING,{
            {
                "As I reach the second tower, relief washes over me like a desert breeze.\n"
                "Wait whats that buried in the sand over there? \n"
                "A tape recorder? Who could possibly live in this hostile environment?\n"
                "**Static** A young woman screams '!NO PLEASE!' **Static** \n"
                "The haunting cry resonates, revealing the desperation of those who once called this place home\n"
                "This message confirms my suspicions; This is not the water planet I expected,\n"
                "but rather a place once rich in life, now transformed\n"
                "The towers, it seems, are remnants of a civilization that once thrived here.\n"
                "Questions multiply as I ponder their fate and the history of this enigmatic world\n"
                "I must get back to my ship to continue my mission.\n"
                "The next tower looms in the distance,\n"
                "a daunting silhouette against the relentless desert horizon.\n"
                "It seems so far away, and I barely made it here\n",
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