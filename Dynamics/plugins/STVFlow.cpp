/*
 * DO NOT EDIT THIS FILE.
 *
 * THIS FILE HAS BEEN AUTOMATICALLY GENERATED.
 * ANY CHANGES MADE TO THE CODE WILL BE LOST.
 *
 * TO MODIFY THE PARAMETER VALUES OR DYNAMICAL
 * EQUATIONS EDIT THE XML FILE (dynamics.xml).
 * 
 */
 
#include "STVFlow.h"
  Integrator* maker()
  {
    return new STVFlowIntegrator;
  }

  class STVFlowProxy
  {
    public:
      STVFlowProxy()
      {
        Factory["STVFlow"] = maker;
      }
  };

  STVFlowProxy p;