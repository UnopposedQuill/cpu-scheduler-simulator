//
// Created by esteban on 19/10/20.
//

#ifndef CLIENT_CONFIGURATION_H
#define CLIENT_CONFIGURATION_H

/**
 * This will be returned upon exiting the configure() function
 * It will contain any necessary configurations for the other mechanisms
 */
struct configuration{
    /**
     * The most important step, if > 0, this configuration will be valid
     */
    short isValid;

    /**
     * The ip address of the server
     */
    char serverAddress[17];

    /**
     * The port of the server
     */
    int port;

    /**
     * 1 for auto mode
     * 0 for manual mode
     */
    int clientMode;
};

/**
 * This will try to read the default configuration named "conf.ini" in the same folder and then return them in
 * the supplied configuration struct
 * @return A Configuration structure representing the configuration for the program
 */
void configure(struct configuration *);

#endif //CLIENT_CONFIGURATION_H
