#include "Fade.h"
#include "system/Sprite.h"
#include "System/Graphics.h"

Fade::Fade()
{
}

Fade::~Fade()
{
    if (sprite != nullptr)
    {
        delete sprite;
        sprite = nullptr;
    }
}

void Fade::Initialize()
{
    sprite = new Sprite("Data/Sprite/White.png");
}


void Fade::StartFadeIn(float duration_)
{
    state = State::FadeIn;

    duration = duration_;
    timer = 0.0f;

    alpha = 1.0f;
}

void Fade::StartFadeOut(float duration_, float holdTime_)
{
    state = State::FadeOut;

    duration = duration_;
    holdDuration = holdTime_;
    timer = 0.0f;

    alpha = 0.0f;
}


void Fade::Update(float elapsedTime)
{
    if (state == State::None)
        return;

    timer += elapsedTime;

    // --- Hold 中 ---
    if (state == State::Hold)
    {
        alpha = 1.0f; // 完全に黒

        if (timer >= holdDuration)
        {
            state = State::None;
        }
        return;
    }

    // --- FadeIn / FadeOut ---
    float t = timer / duration;
    if (t > 1.0f) t = 1.0f;

    if (state == State::FadeIn)
        alpha = 1.0f - t;
    else if (state == State::FadeOut)
        alpha = t;

    // 完了処理
    if (t >= 1.0f)
    {
        if (state == State::FadeOut)
        {
            // 真っ黒になった → Hold へ
            state = State::Hold;
            timer = 0.0f;
            return;
        }

        state = State::None;
    }
}

void Fade::Render(const RenderContext& rc)
{
    if (alpha <= 0.0f)
        return;

    Graphics& graphics = Graphics::Instance();

    float width = static_cast<float>(graphics.GetScreenWidth());
    float height = static_cast<float>(graphics.GetScreenHeight());

    sprite->Render(
        rc,

        0.0f,
        0.0f,
        0.0f,

        width,
        height,

        0.0f,

        0.0f,
        0.0f,
        0.0f,
        alpha
    );
}

bool Fade::IsFinished() const
{
    return state == State::None;
}

bool Fade::IsFading() const
{
    return state != State::None;
}