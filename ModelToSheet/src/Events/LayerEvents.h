#pragma once
#include "Event.h"
#include "Renderer/Model.h"
#include "Renderer/Animation/Animator.h"

#include <string>
#include <memory>

// Model Events - THe UI layer has clicked on a model to be loaded
class ModelLoadStartEvent : public Event {
public:
	ModelLoadStartEvent(const std::string& path, const std::string& name)
		: m_Path(path), m_Name(name) {}

	inline const std::string& GetPath() const { return m_Path; }
	inline const std::string& GetModelName() const { return m_Name; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "ModelLoadStartEvent: " << m_Name;
		return ss.str();
	}

	EVENT_CLASS_TYPE(ModelLoadStart)
	EVENT_CLASS_CATEGORY(EventCategoryLayerModel)
private:
	std::string m_Path;
	std::string m_Name;

};

class ModelLoadCompleteEvent : public Event {
public:
	ModelLoadCompleteEvent(std::shared_ptr<Model> model, std::shared_ptr<Animator> amimator)
		: m_Model(model) , m_Animator(amimator) {}

	inline const std::shared_ptr<Model> GetModel() const { return m_Model; }
	inline const std::shared_ptr<Animator> GetAnimator() const { return m_Animator; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "ModelLoadCompleteEvent";
		return ss.str();
	}

	EVENT_CLASS_TYPE(ModelLoadComplete)
		EVENT_CLASS_CATEGORY(EventCategoryLayerModel)
private:
	std::shared_ptr<Model> m_Model;
	std::shared_ptr<Animator> m_Animator;

};




// Camera Orbit - when the UI layer changes the value assigned to the buttons/slider release this
class CameraOrbitEvent : public Event {
public:
	CameraOrbitEvent(float polar, float azimuthal)
		: m_PolarAngle(polar), m_AzimuthalAngle(azimuthal) {}

	inline float GetPolarAngle() const { return m_PolarAngle; }
	inline float GetAzimuthalAngle() const { return m_AzimuthalAngle; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "CameraOrbitEvent: Polar=" << m_PolarAngle << ", Azimuthal=" << m_AzimuthalAngle;
		return ss.str();
	}

	EVENT_CLASS_TYPE(CameraOrbitChanged)
	EVENT_CLASS_CATEGORY(EventCategoryLayerOther)
private:
	float m_PolarAngle;
	float m_AzimuthalAngle;
};

// Animation Change - A model is currently active, the user has changed the animation they have linked to them.
class AnimationChangeEvent : public Event {
public:
	AnimationChangeEvent(const std::string& animationName)
		: m_AnimationName(animationName) {}

	const std::string& GetAnimationName() const { return m_AnimationName; }

	std::string ToString() const override {
		std::stringstream ss;
		ss << "AnimationChangeEvent: " << m_AnimationName;
		return ss.str();
	}

	EVENT_CLASS_TYPE(AnimationChange)
	EVENT_CLASS_CATEGORY(EventCategoryLayerOther)
private:
	std::string m_AnimationName;
};

//// Shader change event. On the UI the user has changed the type of shader being used
//class ShaderChangeEvent : public Event {
//public:
//	ShaderChangeEvent(FragmentShaderType type)
//		: m_ShaderType(type) {}
//
//	FragmentShaderType GetShaderType() const { return m_ShaderType; }
//
//	std::string ToString() const override {
//		std::stringstream ss;
//		ss << "ShaderChangeEvent: " << static_cast<int>(m_ShaderType);
//		return ss.str();
//	}
//
//	EVENT_CLASS_TYPE(ShaderChange)
//	EVENT_CLASS_CATEGORY(EventCategoryLayer)
//private:
//	FragmentShaderType m_ShaderType;
//};