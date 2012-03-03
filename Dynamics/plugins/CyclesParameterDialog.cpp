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
 
#include "CyclesParameterDialog.h"
#include "GLMotif/WidgetFactory.h"
#include "IntegrationStepSize.h"
#include "VruiStreamManip.h"

GLMotif::PopupWindow* CyclesParameterDialog::createDialog()
{
  WidgetFactory factory;
  GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", "Cycles Parameters");

  GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 3);
  factory.setLayout(parameterDialog);

  factory.createLabel("SParameterLabel", "s");

  currentSValue=factory.createTextField("CurrentSValue", 10);
  currentSValue->setLabel(".12");

  sParameterSlider=factory.createSlider("SParameterSlider", 15.0);
  sParameterSlider->setValueRange(-3, 3, 0.1);
  sParameterSlider->setValue(.12);
  sParameterSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);

  factory.createLabel("Dk_yParameterLabel", "dk_Y");

  currentDk_yValue=factory.createTextField("CurrentDk_yValue", 10);
  currentDk_yValue->setLabel("-3");

  dk_YParameterSlider=factory.createSlider("Dk_yParameterSlider", 15.0);
  dk_YParameterSlider->setValueRange(-10, 10, 0.1);
  dk_YParameterSlider->setValue(-3);
  dk_YParameterSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);

  factory.createLabel("Dk_xzParameterLabel", "dk_XZ");

  currentDk_xzValue=factory.createTextField("CurrentDk_xzValue", 10);
  currentDk_xzValue->setLabel("0");

  dk_XZParameterSlider=factory.createSlider("Dk_xzParameterSlider", 15.0);
  dk_XZParameterSlider->setValueRange(-.1, .05, 0.001);
  dk_XZParameterSlider->setValue(0);
  dk_XZParameterSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);


  factory.createLabel("StepSizeLabel", "step size");
  stepSizeValue=factory.createTextField("StepSizeValue", 10);
  double step_size = IntegrationStepSize::instance()->getSavedValue("Cycles");
  if (step_size > 0.0) stepSizeValue->setLabel(toString(step_size).c_str());
  else stepSizeValue->setLabel("0.01");
  stepSizeSlider=factory.createSlider("StepSizeSlider", 15.0);
  stepSizeSlider->setValueRange(0.0001, 0.05, 0.0001);
  if (step_size > 0.0) stepSizeSlider->setValue(step_size);
  else stepSizeSlider->setValue(0.01);
  stepSizeSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);


  factory.createLabel("EvaluationLabel", "Evaluation Method");
  GLMotif::ToggleButton* exactEvalToggle=factory.createCheckBox("ExactEvalToggle", "Exact", true);
  GLMotif::ToggleButton* gridEvalToggle=factory.createCheckBox("GridEvalToggle", "Interpolated Grid");
  // assign line style toggle callbacks
  exactEvalToggle->getValueChangedCallbacks().add(this, &CyclesParameterDialog::evalTogglesCallback);
  gridEvalToggle->getValueChangedCallbacks().add(this, &CyclesParameterDialog::evalTogglesCallback);

  factory.createLabel("xSpacingLabel", "x-Grid Spacing");
  currentXValue=factory.createTextField("xTextField", 12);
  currentXValue->setLabel("1.0");
  currentXValue->setCharWidth(5);
  currentXValue->setPrecision(5);
  xSpacingSlider=factory.createSlider("XSpacingSlider", 15.0);
  xSpacingSlider->setValueRange(.001, 2.0, 0.001);
  xSpacingSlider->setValue(1.0);
  xSpacingSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);

  factory.createLabel("ySpacingLabel", "y-Grid Spacing");
  currentYValue=factory.createTextField("yTextField", 12);
  currentYValue->setLabel("1.0");
  currentYValue->setCharWidth(5);
  currentYValue->setPrecision(5);  ySpacingSlider=factory.createSlider("YSpacingSlider", 15.0);
  ySpacingSlider->setValueRange(.001, 2.0, 0.001);
  ySpacingSlider->setValue(1.0);
  ySpacingSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);

  factory.createLabel("zSpacingLabel", "z-Grid Spacing");
  currentZValue=factory.createTextField("zTextField", 12);
  currentZValue->setLabel("1.0");
  currentZValue->setCharWidth(5);
  currentZValue->setPrecision(5);
  zSpacingSlider=factory.createSlider("ZSpacingSlider", 15.0);
  zSpacingSlider->setValueRange(.001, 2.0, 0.001);
  zSpacingSlider->setValue(1.0);
  zSpacingSlider->getValueChangedCallbacks().add(this, &CyclesParameterDialog::sliderCallback);

  // add toggles to array for radio-button behavior
  evalToggles.push_back(exactEvalToggle);
  evalToggles.push_back(gridEvalToggle);

  parameterDialog->manageChild();
  return parameterDialogPopup;
}

void CyclesParameterDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
  double value = cbData->value;

  char buff[10];
  snprintf(buff, sizeof(buff), "%3.2f", value);

  if (strcmp(cbData->slider->getName(), "SParameterSlider")==0)
    {
      currentSValue->setLabel(buff);
      model->setValue("s", value);
    }
  else if (strcmp(cbData->slider->getName(), "Dk_yParameterSlider")==0)
    {
      currentDk_yValue->setLabel(buff);
      model->setValue("dk_Y", value);
    }
  else if (strcmp(cbData->slider->getName(), "Dk_xzParameterSlider")==0)
    {
      currentDk_xzValue->setLabel(buff);
      model->setValue("dk_XZ", value);
    }
  else if (strcmp(cbData->slider->getName(), "StepSizeSlider")==0)
  {
    snprintf(buff, sizeof(buff), "%6.4f", value);
    stepSizeValue->setLabel(buff);
    IntegrationStepSize::instance()->setValue(value);
    IntegrationStepSize::instance()->saveValue("Cycles", value);
  }

  else if (strcmp(cbData->slider->getName(), "XSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentXValue->setLabel(buff);
      model->setSpacing(0, value);
    }
  else if (strcmp(cbData->slider->getName(), "YSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentYValue->setLabel(buff);
      model->setSpacing(1, value);
    }
  else if (strcmp(cbData->slider->getName(), "ZSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentZValue->setLabel(buff);
      model->setSpacing(2, value);
    }
}

void CyclesParameterDialog::evalTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   std::string name=cbData->toggle->getName();

   if (name == "ExactEvalToggle")
   {
      model->setSimulate(false);
   }
   else if (name == "GridEvalToggle")
   {
      model->setSimulate(true);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=evalToggles.begin(); button != evalToggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0 and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);

}
