/*
 * Copyright © 2012, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All rights reserved.
 *
 * The NASA Tensegrity Robotics Toolkit (NTRT) v1 platform is licensed
 * under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
*/

/**
 * @file AppLaikaCombinedMotion.cpp
 * @brief Contains the definition function main() for AppLaikaCombinedMotion
 * which builds a horizontal spine structure defined in YAML, with a rotating vertebra
 * @author Drew Sabelhaus
 * $Id$
 */

// This application
#include "yamlbuilder/TensegrityModel.h"
// This library
#include "core/terrain/tgBoxGround.h"
#include "core/tgModel.h"
#include "core/tgRod.h"
#include "core/tgSimulation.h"
#include "core/tgSimViewGraphics.h"
#include "core/tgWorld.h"
// For tracking positions:
#include "sensors/tgDataLogger2.h"
#include "sensors/tgSphereSensorInfo.h"
#include "sensors/tgRodSensorInfo.h"
// Bullet Physics
#include "LinearMath/btVector3.h"
// The C++ Standard Library
#include <iostream>
#include <string>
#include <vector>

/**
 * The entry point.
 * @param[in] argc the number of command-line arguments
 * @param[in] argv argv[0] is the executable name
 * @param[in] argv argv[1] is the path of the YAML encoded structure
 * @return 0
 */
int main(int argc, char** argv)
{
    // For this YAML parser app, need to check that an argument path was
    // passed in.
    if (argv[1] == NULL)
    {
      throw std::invalid_argument("No arguments passed in to the application. You need to specify which YAML file you wouldd like to build.");
    }
  
    // create the ground and world. Specify ground rotation in radians
    const double yaw = 0.0;
    const double pitch = 0.0;
    const double roll = 0.0;
    const tgBoxGround::Config groundConfig(btVector3(yaw, pitch, roll));
    // the world will delete this
    tgBoxGround* ground = new tgBoxGround(groundConfig);

    const tgWorld::Config config(98.1); // gravity, dm/sec^2
    tgWorld world(config, ground);

    // create the view
    const double timestep_physics = 0.0001; // seconds
    //const double timestep_physics = 0.001;
    const double timestep_graphics = 1.f/60.f; // seconds
    tgSimViewGraphics view(world, timestep_physics, timestep_graphics);

    // create the simulation
    tgSimulation simulation(view);

    // create the models with their controllers and add the models to the simulation
    // This constructor for TensegrityModel takes the 'debugging' flag as the
    // second argument.
    TensegrityModel* const myModel = new TensegrityModel(argv[1],true);

    // Add the model to the world
    simulation.addModel(myModel);

    // Let's log info from the spheres (bottom of Laika's feet.)
    // has to end with the prefix to the log file name.
    std::string log_filename = "~/NTRTsim_logs/CompoundSensorDebugDemo";
    double samplingTimeInterval = 0.1;
    tgDataLogger2* myDataLogger = new tgDataLogger2(log_filename, samplingTimeInterval);
    // add the model to the data logger
    myDataLogger->addSenseable(myModel);
    // Make it so the data logger can dispatch sphere sensors
    tgSphereSensorInfo* mySphereSensorInfo = new tgSphereSensorInfo();
    //DEBUGGING: rods too
    tgRodSensorInfo* myRodSensorInfo = new tgRodSensorInfo();
    myDataLogger->addSensorInfo(mySphereSensorInfo);
    myDataLogger->addSensorInfo(myRodSensorInfo);
    // Add the data logger to the simulation.
    simulation.addDataManager(myDataLogger);
    
    // Finally, run the simulation.
    simulation.run();

    // teardown is handled by delete
    return 0;
}