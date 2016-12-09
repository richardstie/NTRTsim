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

#ifndef JSON_MG_FEEDBACK_CONTROL_FM4_H
#define JSON_MG_FEEDBACK_CONTROL_FM4_H

/**
 * @file JSONMGFeedbackControlFM4.h
 * @brief A controller for the template class BaseQuadModelLearning,
 * modified for use on a quadruped
 * @author Brian Mirletz, Dawn Hustig-Schultz
 * @version 1.1.0
 * $Id$
 */

#include "dev/dhustigschultz/BP_SC_NoLegs_Stats/JSONQuadCPGControl.h"

#include <json/value.h>

// Forward Declarations
class neuralNetwork;
class tgSpringCableActuator;

/**
 * JSONFeedbackControl learns the parameters for a CPG system on a
 * spine like tensegrity structure specified as a BaseQuadModelLearning. Parameters are generated by
 * AnnealEvolution and used in the CPGEquations family of classes.
 * tgImpedanceController controllers are used for the detailed muscle control.
 * Due to the number of parameters, the learned parameters are split
 * into one config file for the nodes and another for the CPG's "edges"
 */
class JSONMGFeedbackControlFM4 : public JSONQuadCPGControl
{
public:

struct Config : public JSONQuadCPGControl::Config
    {
    public:
        /**
         * The only constructor. 
         */
        Config( int ss,
        int tm,
        int om,
        int param,
        int segnum = 6,
        double ct = 0.1,
        double la = 0,
        double ha = 30,
        double lp = -1 * M_PI,
        double hp = M_PI,
        double kt = 0.0,
        double kp = 1000.0,
        double kv = 100.0,
        bool def = true,
        double cl = 10.0,
        double lf = 0.0,
        double hf = 30.0,
        double ffMin = 0.0,
        double ffMax = 0.0,
        double afMin = 0.0,
        double afMax = 0.0,
        double pfMin = 0.0,
        double pfMax = 0.0,
	double maxH = 60.0, //May need to tune this value more
	double minH = 1.0   //Perhaps same
        );
        
        const double freqFeedbackMin;
        const double freqFeedbackMax;
        const double ampFeedbackMin;
        const double ampFeedbackMax;
        const double phaseFeedbackMin;
        const double phaseFeedbackMax;

	const double maxHeight;
	const double minHeight;
        
        // Values to be filled in by JSON file during onSetup
        int numStates;
        int numActions;
        
    };

    JSONMGFeedbackControlFM4(JSONMGFeedbackControlFM4::Config config,	
							std::string args,
							std::string resourcePath = "");
    
    virtual ~JSONMGFeedbackControlFM4();
    
    virtual void onSetup(BaseQuadModelLearning& subject);
    
    virtual void onStep(BaseQuadModelLearning& subject, double dt);
    
    virtual void onTeardown(BaseQuadModelLearning& subject);

	
protected:

    virtual void setupCPGs(BaseQuadModelLearning& subject, array_2D nodeActions, array_4D edgeActions);
    
    virtual array_2D scaleNodeActions (Json::Value actions);
    
    std::vector<double> getFeedback(BaseQuadModelLearning& subject);
    
    std::vector<double> getCableState(const tgSpringCableActuator& cable);
    
    std::vector<double> transformFeedbackActions(std::vector< std::vector<double> >& actions);
    
    JSONMGFeedbackControlFM4::Config m_config;

    std::vector<tgCPGActuatorControl*> m_spineControllers;
    
    /// @todo generalize this if we need more than one
    neuralNetwork* nn;

    double m_totalTime;
    
};

#endif // JSON_QUAD_FEEDBACK_CONTROL_H