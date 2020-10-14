//
// Created by esteban on 14/10/20.
//

#include "configuration.h"

struct configuration{

    /**
     * The most important step, if > 0, this configuration will be valid
     */
    short isValid;
};

void configure(struct configuration * conf){
    conf->isValid = 1;
}