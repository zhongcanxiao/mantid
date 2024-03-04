// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "FitTypes.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/IFunction.h"

namespace MantidQt::CustomInterfaces::IDA {

namespace ConvTypes {

std::map<FitType, bool> FitTypeQDepends = std::map<FitType, bool>({{FitType::None, false},
                                                                   {FitType::TeixeiraWater, true},
                                                                   {FitType::FickDiffusion, true},
                                                                   {FitType::ChudleyElliot, true},
                                                                   {FitType::HallRoss, true},
                                                                   {FitType::StretchedExpFT, false},
                                                                   {FitType::DiffSphere, true},
                                                                   {FitType::ElasticDiffSphere, true},
                                                                   {FitType::InelasticDiffSphere, true},
                                                                   {FitType::DiffRotDiscreteCircle, true},
                                                                   {FitType::InelasticDiffRotDiscreteCircle, true},
                                                                   {FitType::ElasticDiffRotDiscreteCircle, true},
                                                                   {FitType::IsoRotDiff, true},
                                                                   {FitType::ElasticIsoRotDiff, true},
                                                                   {FitType::InelasticIsoRotDiff, true}});

std::unordered_map<std::string, FitType>
    FitTypeStringToEnum({{"TeixeiraWaterSQE", FitType::TeixeiraWater},
                         {"FickDiffusionSQE", FitType::FickDiffusion},
                         {"ChudleyElliotSQE", FitType::ChudleyElliot},
                         {"HallRossSQE", FitType::HallRoss},
                         {"StretchedExpFT", FitType::StretchedExpFT},
                         {"DiffSphere", FitType::DiffSphere},
                         {"ElasticDiffSphere", FitType::ElasticDiffSphere},
                         {"InelasticDiffSphere", FitType::InelasticDiffSphere},
                         {"DiffRotDiscreteCircle", FitType::DiffRotDiscreteCircle},
                         {"InelasticDiffRotDiscreteCircle", FitType::InelasticDiffRotDiscreteCircle},
                         {"ElasticDiffRotDiscreteCircle", FitType::ElasticDiffRotDiscreteCircle},
                         {"IsoRotDiff", FitType::IsoRotDiff},
                         {"ElasticIsoRotDiff", FitType::ElasticIsoRotDiff},
                         {"InelasticIsoRotDiff", FitType::InelasticIsoRotDiff}});

} // namespace ConvTypes

template <>
std::map<ConvTypes::FitType, TemplateSubTypeDescriptor> TemplateSubTypeImpl<ConvTypes::FitType>::g_typeMap{
    {ConvTypes::FitType::None, {"None", "", {ParamID::NONE, ParamID::NONE}}},
    {ConvTypes::FitType::TeixeiraWater,
     {"Teixeira Water SQE", "TeixeiraWaterSQE", {ParamID::TW_HEIGHT, ParamID::TW_CENTRE}}},
    {ConvTypes::FitType::FickDiffusion,
     {"Fick Diffusion SQE", "FickDiffusionSQE", {ParamID::FD_HEIGHT, ParamID::FD_CENTRE}}},
    {ConvTypes::FitType::ChudleyElliot,
     {"Chudley-Elliot SQE", "ChudleyElliotSQE", {ParamID::CE_HEIGHT, ParamID::CE_CENTRE}}},
    {ConvTypes::FitType::HallRoss, {"Hall-Ross SQE", "HallRossSQE", {ParamID::HR_HEIGHT, ParamID::HR_CENTRE}}},
    {ConvTypes::FitType::StretchedExpFT,
     {"StretchedExpFT", "StretchedExpFT", {ParamID::SE_HEIGHT, ParamID::SE_CENTRE}}},
    {ConvTypes::FitType::DiffSphere, {"DiffSphere", "DiffSphere", {ParamID::DP_INTENSITY, ParamID::DP_SHIFT}}},
    {ConvTypes::FitType::ElasticDiffSphere,
     {"ElasticDiffSphere", "ElasticDiffSphere", {ParamID::EDP_HEIGHT, ParamID::EDP_RADIUS}}},
    {ConvTypes::FitType::InelasticDiffSphere,
     {"InelasticDiffSphere", "InelasticDiffSphere", {ParamID::IDP_INTENSITY, ParamID::IDP_SHIFT}}},
    {ConvTypes::FitType::DiffRotDiscreteCircle,
     {"DiffRotDiscreteCircle", "DiffRotDiscreteCircle", {ParamID::DRDC_INTENSITY, ParamID::DRDC_SHIFT}}},
    {ConvTypes::FitType::InelasticDiffRotDiscreteCircle,
     {"InelasticDiffRotDiscreteCircle",
      "InelasticDiffRotDiscreteCircle",
      {ParamID::IDRDC_INTENSITY, ParamID::IDRDC_SHIFT}}},
    {ConvTypes::FitType::ElasticDiffRotDiscreteCircle,
     {"ElasticDiffRotDiscreteCircle", "ElasticDiffRotDiscreteCircle", {ParamID::EDRDC_HEIGHT, ParamID::EDRDC_RADIUS}}},
    {ConvTypes::FitType::IsoRotDiff, {"IsoRotDiff", "IsoRotDiff", {ParamID::IRD_HEIGHT, ParamID::IRD_CENTRE}}},
    {ConvTypes::FitType::ElasticIsoRotDiff,
     {"ElasticIsoRotDiff", "ElasticIsoRotDiff", {ParamID::EIRD_HEIGHT, ParamID::EIRD_RADIUS}}},
    {ConvTypes::FitType::InelasticIsoRotDiff,
     {"InelasticIsoRotDiff", "InelasticIsoRotDiff", {ParamID::IIRD_HEIGHT, ParamID::IIRD_CENTRE}}},
};
template <>
std::map<ConvTypes::LorentzianType, TemplateSubTypeDescriptor>
    TemplateSubTypeImpl<ConvTypes::LorentzianType>::g_typeMap{
        {ConvTypes::LorentzianType::None, {"None", "", {ParamID::NONE, ParamID::NONE}}},
        {ConvTypes::LorentzianType::OneLorentzian,
         {"One Lorentzian", "Lorentzian", {ParamID::LOR1_AMPLITUDE, ParamID::LOR1_FWHM}}},
        {ConvTypes::LorentzianType::TwoLorentzians,
         {"Two Lorentzians", "Lorentzian", {ParamID::LOR2_AMPLITUDE_1, ParamID::LOR2_FWHM_1, ParamID::LOR2_FWHM_2}}},
    };

template <>
std::map<ConvTypes::BackgroundType, TemplateSubTypeDescriptor>
    TemplateSubTypeImpl<ConvTypes::BackgroundType>::g_typeMap{
        {ConvTypes::BackgroundType::None, {"None", "", {ParamID::NONE, ParamID::NONE}}},
        {ConvTypes::BackgroundType::Flat,
         {"FlatBackground", "FlatBackground", {ParamID::FLAT_BG_A0, ParamID::FLAT_BG_A0}}},
        {ConvTypes::BackgroundType::Linear,
         {"LinearBackground", "LinearBackground", {ParamID::LINEAR_BG_A0, ParamID::LINEAR_BG_A1}}},
    };

template <>
std::map<bool, TemplateSubTypeDescriptor> TemplateSubTypeImpl<bool>::g_typeMap{
    {false, {"None", "", {ParamID::NONE, ParamID::NONE}}},
    {true, {"DeltaFunction", "DeltaFunction", {ParamID::DELTA_HEIGHT, ParamID::DELTA_CENTER}}},
};

template <>
std::map<ConvTypes::TempCorrectionType, TemplateSubTypeDescriptor>
    TemplateSubTypeImpl<ConvTypes::TempCorrectionType>::g_typeMap{
        {ConvTypes::TempCorrectionType::None, {"None", "", {ParamID::NONE}}},
        {ConvTypes::TempCorrectionType::Exponential,
         {"Temp Correction", "ConvTempCorrection", {ParamID::TEMPERATURE, ParamID::TEMPERATURE}}},
    };
} // namespace MantidQt::CustomInterfaces::IDA
