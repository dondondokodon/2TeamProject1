#pragma once

#include <memory>

class Sprite;
struct RenderContext;

class Fade
{
public:

    enum class State
    {
        None,
        FadeIn,
        FadeOut,
        Hold,  
    };


public:

    Fade();
    ~Fade();

	void Initialize();

    void Update(float elapsedTime);
    void Render(const RenderContext& rc);

    void StartFadeIn(float duration);
    void StartFadeOut(float duration_, float holdTime_);

    bool IsFinished() const;
    bool IsFading() const;

private:

    Sprite* sprite = nullptr;

    State state = State::None;

    float alpha = 0.0f;

    float timer = 0.0f;
    float duration = 1.0f;
    float holdDuration = 0.0f; // 黒画面を維持する秒数

};