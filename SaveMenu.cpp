#include "pch.h"
#include "SaveMenu.h"
#include "Texture.h"
#include "SoundEffect.h"

SaveMenu::SaveMenu(float windowWidth, float windowHeight)
    : m_pBackground{ new Texture{ "Resources/Images/SaveMenu.png" } }
    , m_pBottleTexture{ new Texture{ "Resources/Images/PotionIcon.png" } }
    , m_pVolumeBarTexture{ new Texture{ "Resources/Images/VolumeBar.png" } }
    , m_Zoom{ windowHeight / m_pBackground->GetHeight() }
    , m_WindowWidth{ windowWidth }
    , m_pMoveButtonSound{ new SoundEffect{ "Resources/Sounds/SaveMenuSelect.mp3" } }
{
}

SaveMenu::~SaveMenu()
{
    delete m_pBackground;
    m_pBackground = nullptr;
    delete m_pBottleTexture;
    m_pBottleTexture = nullptr;
    delete m_pMoveButtonSound;
    m_pMoveButtonSound = nullptr;
    delete m_pVolumeBarTexture;
    m_pVolumeBarTexture = nullptr;
}
void SaveMenu::MoveButton()
{
    ++m_CurButton %= 2;
    m_pMoveButtonSound->Play(0);
}

void SaveMenu::Reset()
{
    m_CurButton = 0;
}

void SaveMenu::SetVolume(int volume, bool playSound)
{
    m_pMoveButtonSound->SetVolume(volume);
    if (playSound)
    {
        m_pMoveButtonSound->Play(0);
    }
}

bool SaveMenu::Select() const
{
    return m_CurButton == 1;
}

void SaveMenu::Draw(int volume) const
{
    const float distanceBetweenOptions{ 16.0f };
    const Point2f curBottlePos
    {
        80.0f,
        177.0f - m_CurButton * distanceBetweenOptions
    };

    glPushMatrix();
    {
        glTranslatef(m_WindowWidth / 2.0f - m_pBackground->GetWidth() * m_Zoom / 2.0f, 0.0f, 0.0f);
        glScalef(m_Zoom, m_Zoom, 1.0f);
        m_pBackground->Draw();
        m_pBottleTexture->Draw(curBottlePos);

        const Point2f volumeStartPos
        {
            116.0f,
            42.0f
        };
        const int maxVolume{ 120 };
        const int nrVolumeSteps{ 10 };
        const int volumeStep{ maxVolume / nrVolumeSteps };

        const float volumeTextureSize{ 8.0f };

        Rectf srcVolumeRect
        {
            0.0f,
            volumeTextureSize,
            volumeTextureSize,
            volumeTextureSize
        };

        for (int i{ 0 }; i < nrVolumeSteps; ++i)
        {
            const int stepMin{ volumeStep * i };
            const int stepMax{ stepMin + volumeStep };

            if (volume >= stepMin && volume < stepMax)
            {
                srcVolumeRect.left = volumeTextureSize;
            }
            else
            {
                srcVolumeRect.left = 0.0f;
            }

            const Point2f curPosition
            {
                volumeStartPos.x + volumeTextureSize * i,
                volumeStartPos.y
            };

            m_pVolumeBarTexture->Draw(curPosition, srcVolumeRect);
        }
    }
    glPopMatrix();
}
