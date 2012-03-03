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
 
#ifndef DRIVENVANDERPOL_INCLUDED_
#define DRIVENVANDERPOL_INCLUDED_

#include "BaseModel.h"
#include "RungeKutta4.h"

#include "DrivenVanDerPolParameterDialog.h"

class DrivenVanDerPol : public DynamicalModel
{
  Scalar b;
  Scalar A;
  Scalar w;
public:
  DrivenVanDerPol(Scalar B, Scalar A, Scalar W)
     : b(B), A(A), w(W)
  { }

  virtual ~DrivenVanDerPol() { }

  virtual Vector exact(const Point& p) const
    {
      return Vector(p[2],w,b*(1.-p[0]*p[0])*p[2] - p[0] + A*cos(p[1]));
    }

private:
  virtual void _setValue(const std::string& name, Scalar value)
    {
      if      (name == "b") b = value;
      else if (name == "A") A = value;
      else if (name == "w") w = value;
    }
};

class DrivenVanDerPolIntegrator : public Integrator
{
  DrivenVanDerPol* dynamics;
  RungeKutta4<DrivenVanDerPol>* integrator;
public:
  DrivenVanDerPolIntegrator()
    {
      dynamics = new DrivenVanDerPol(2., 3.0, 1.5);
      double stepSize = 0.01;
      integrator = new RungeKutta4<DrivenVanDerPol>(*dynamics, stepSize);
    }

  virtual ~DrivenVanDerPolIntegrator()
    {
      delete dynamics;
      delete integrator;
    }

  inline Vector step(const Point& p)
    {
      return integrator->step(p);
    }

  virtual unsigned int getModelVersion()
    {
      return dynamics->getModelVersion();
    }

  virtual CaveDialog* createParameterDialog(GLMotif::PopupMenu *parent)
    {
      return new DrivenVanDerPolParameterDialog(parent, dynamics);
    }

  virtual Vrui::Point center() const
    {
      return Vrui::Point(0.0, 0.0, 3.14159);
    }
};

#endif