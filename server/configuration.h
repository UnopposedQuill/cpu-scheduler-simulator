//
// Created by esteban on 14/10/20.
//

#ifndef SERVER_CONFIGURATION_H
#define SERVER_CONFIGURATION_H

/**
 * This will be returned upon exiting the configure() function
 * It will contain any necessary configurations for the other mechanisms
 */
struct configuration;

/**
 * This will try to read the default configuration named "conf.ini" in the same folder and then return them in
 * the supplied configuration struct
 * @return A Configuration structure representing the configuration for the program
 */
void configure(struct configuration *);


#endif //SERVER_CONFIGURATION_H
