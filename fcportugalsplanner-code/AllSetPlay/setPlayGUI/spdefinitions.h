// -*-c++-*-


#ifndef SPDEFINITIONS_H
#define SPDEFINITIONS_H

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

enum setPlaySituation { play_on, kick_off, throw_in,
                   dir_free_kick, ind_free_kick,
                   corner_kick, goal_kick, keeper_catch };

enum setPlayType { off, def };

enum setPlaySide { rightSide, leftSide };

static int setPlayRegionsNumber = 22;

enum setPlayRegions { field, our_middle_field, their_middle_field,
              far_left, mid_left , centre_left, centre_right, mid_right, far_right, // divides the field in 6 horizontal parts
              left_region, right_region,
              our_back, our_middle, our_front, their_back, their_middle, their_front, // divides the field in 6 vertical parts
              our_goal_box, our_penalty_box,
              opp_goal_box, opp_penalty_box,
              point};

static const char * setPlayRegionsNames[] = { "field", "our_middle_field", "their_middle_field",
              "far_left", "mid_left" , "centre_left", "centre_right", "mid_right", "far_right",
              "left", "right",
              "our_back", "our_middle", "our_front", "their_back", "their_middle", "their_front",
              "our_goal_box", "our_penalty_box",
              "opp_goal_box", "opp_penalty_box",
              "point"};

enum setPlayPlayerConditions { ppos, bowner, nearOffsideLine, canShoot, canPassPlayer, canPassRegion };
enum setPlayOtherConditions { ourBallCond, theirBallCond, playOffCond };

static const char * setPlayPlayerConditionNames[] = { "On position", "Is the ball owner" , "Near offside line",
                                                "Can shoot", "Can pass"};



#endif // SPDEFINITIONS_H
