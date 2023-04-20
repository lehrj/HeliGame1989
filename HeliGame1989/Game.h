//
// Game.h
//

#pragma once
#include "AnimatedTexture.h"
#include "Keyboard.h"
#include "SpriteSheet.h"
#include "WICTextureLoader.h"

#include "Camera.h"
#include "DebugData.h"
#include "Environment.h"
#include "Lighting.h"
#include "sounds.h"
#include "Vehicle.h"
#include <sstream>


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:
    Game() noexcept;
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const noexcept;

    // Audio
    void OnNewAudioDevice() { m_retryAudio = true; };

private:
    void AudioPlayMusic(XACT_WAVEBANK_AUDIOBANK aSFX);
    void AudioPlaySFX(XACT_WAVEBANK_AUDIOBANK aSFX);

    void Clear();
    void CreateDevice();
    void CreateResources();

    void DrawCameraFocus();
    void DrawDebugLinesVectorNew();
    void DrawDebugDataUI();
    void DrawDebugLines(const DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor);
    void DrawDebugNormalLines(const DirectX::SimpleMath::Vector3 aPos, const DirectX::XMVECTORF32 aColor);
    void DrawDebugLinesVector();
    void DrawGridForStartScreen();
    void DrawGamePlayStart();
    void DrawIntroScene();
    void DrawLightBar();
    void DrawLightFocus2();
    void DrawLightFocus1();
    void DrawLightFocus3();
    void DrawLoadScreen();
    void DrawLogoScreen();
    void DrawMenuEnvironmentSelect();
    void DrawMenuMain();
    void DrawSky();
    void DrawStartScreen();
    void DrawTeaserScreen();
    void DrawTerrain();
    void DrawTerrain2();
    void DrawTerrainNew(Terrain& aTerrain);
    void DrawTerrainNormals();
    void DrawTimer();
    void DrawUIIntroScreen();
    void DrawWorld();
    void DrawWorldCube();
    void DrawWorldCubeTextured();

    void InitializeDebugWorldGrid();
    bool InitializeTerrainArray();
    bool InitializeTerrainArray2();

    bool InitializeTerrainArrayNew(Terrain& aTerrain);
    bool InitializeTerrainArrayStartScreen(Terrain& aTerrain);

    void InitializeWorldGrid();
    void OnDeviceLost();
    void Present();
    void Render();
    void ResetGamePlay();

    void SetFogVals(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals2(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals3(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetTerrainGridDimmer(const float aDimmerVal);
    void TerrainDimmer();

    void DrawDebugValue();
    void DrawDebugVehicleData();
    void TestDraw();

    void Update(DX::StepTimer const& aTimer);
    void UpdateInput(DX::StepTimer const& aTimer);

    Camera* m_camera;
    std::shared_ptr<DebugData> m_debugData;
    Vehicle* m_vehicle;
    Environment* m_environment;
    Lighting* m_lighting;

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;
    const int                                       m_outputWidthDefault = 1600;
    const int                                       m_outputHeightDefault = 900;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

    using VertexType = DirectX::VertexPositionNormalColorTexture;
    using VertexType2 = DirectX::VertexPositionNormalColor;
    using VertexType3 = DirectX::VertexPositionColor;

    std::unique_ptr<DirectX::CommonStates>          m_states;
    std::shared_ptr<DirectX::NormalMapEffect>       m_effect;
    std::shared_ptr<DirectX::BasicEffect>           m_effect2;
    std::shared_ptr<DirectX::BasicEffect>           m_effect3;
    std::shared_ptr<DirectX::NormalMapEffect>           m_effect4;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType2>> m_batch2;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType3>> m_batch3;
    std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch4;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureSky;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specular;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTest;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTest;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularJI;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureJI;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapBMW;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapBMW2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularBMW;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureBMW;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapAutoGame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularAutoGame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureAutoGame;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTitle;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTitle;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTitle;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapTeaser;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTeaser;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureTeaser;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout2;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout3;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout4;

    std::shared_ptr<DirectX::IEffect>               m_lightEffect;

    std::unique_ptr<DirectX::GeometricPrimitive>    m_skyShape;
    float                                           m_skyRotation = 0.0f;
    std::unique_ptr<DirectX::GeometricPrimitive>    m_testShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    m_testShape2;
    std::unique_ptr<DirectX::GeometricPrimitive>    m_titleShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    m_moonShape;

    DirectX::SimpleMath::Matrix                     m_world;
    DirectX::SimpleMath::Matrix                     m_view;
    DirectX::SimpleMath::Matrix                     m_proj;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_raster; // WLJ anti-aliasing

    // WLJ added for drawing projectile and swing  
    int                                             m_swingPathStep = 0;
    int                                             m_projectilePathStep = 0;

    // WLJ add for mouse and keybord interface
    std::unique_ptr<DirectX::Keyboard>              m_keyboard;
    std::unique_ptr<DirectX::Mouse>                 m_mouse;
    DirectX::Keyboard::KeyboardStateTracker         m_kbStateTracker;
    std::unique_ptr<DirectX::GamePad>               m_gamePad;
    DirectX::GamePad::ButtonStateTracker            m_gamePadTracker;
    const float                                     m_gamePadInputDeadZone = 0.01f;
    const float                                     m_gamePadInputRatePitch = 1.0f;
    const float                                     m_gamePadInputRateRoll = 1.0f;
    const float                                     m_gamePadInputRateYaw = 1.0f;
    const float                                     m_gamePadInputRateCollective = 0.02f;
    const float                                     m_gamePadInputRateThrottle = 0.4f;

    // WLJ added for displaying text
    std::unique_ptr<DirectX::SpriteFont>            m_font;
    std::unique_ptr<DirectX::SpriteFont>            m_titleFont;
    std::unique_ptr<DirectX::SpriteFont>            m_bitwiseFont;
    DirectX::SimpleMath::Vector2                    m_fontPos;
    DirectX::SimpleMath::Vector2                    m_fontPos2;
    DirectX::SimpleMath::Vector2                    m_fontPosDebug;
    DirectX::SimpleMath::Vector2                    m_fontMenuPos;
    DirectX::SimpleMath::Vector2                    m_bitwiseFontPos;
    std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch;

    // Character Textures
    std::unique_ptr<AnimatedTexture>                m_character;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_characterTexture;
    DirectX::SimpleMath::Vector2                    m_characterPos;

    std::unique_ptr<AnimatedTexture>                m_pacSprite;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pacTexture;
    DirectX::SimpleMath::Vector2                    m_pacPos;

    std::unique_ptr<AnimatedTexture>                m_character0;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character0Texture;
    DirectX::SimpleMath::Vector2                    m_character0Pos;

    std::unique_ptr<AnimatedTexture>                m_character1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character1Texture;
    DirectX::SimpleMath::Vector2                    m_character1Pos;

    std::unique_ptr<AnimatedTexture>                m_character2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_character2Texture;
    DirectX::SimpleMath::Vector2                    m_character2Pos;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_characterBackgroundTexture;
    DirectX::SimpleMath::Vector2                    m_characterBackgroundPos;
    DirectX::SimpleMath::Vector2                    m_characterBackgroundOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectCalmTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectCalmPos;
    DirectX::SimpleMath::Vector2                    m_environSelectCalmOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectBreezyTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectBreezyPos;
    DirectX::SimpleMath::Vector2                    m_environSelectBreezyOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_environSelectAlienTexture;
    DirectX::SimpleMath::Vector2                    m_environSelectAlienPos;
    DirectX::SimpleMath::Vector2                    m_environSelectAlienOrigin;

    // WLJ Intro screen logos
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bmwLogoTexture;
    DirectX::SimpleMath::Vector2                    m_bmwLogoPos;
    DirectX::SimpleMath::Vector2                    m_bmwLogoOrigin;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jiLogoTexture;
    DirectX::SimpleMath::Vector2                    m_jiLogoPos;
    DirectX::SimpleMath::Vector2                    m_jiLogoOrigin;

    // WLJ variable to hold selected menu item
    int                                             m_menuSelect = 0;

    enum class GameState
    {
        GAMESTATE_INTROSCREEN,
        GAMESTATE_STARTSCREEN,
        GAMESTATE_MAINMENU,
        GAMESTATE_CHARACTERSELECT,
        GAMESTATE_COURSESELECT,
        GAMESTATE_ENVIRONTMENTSELECT,
        GAMESTATE_GAMEPLAY,
        GAMESTATE_GAMEPLAYSTART,
        GAMESTATE_TEASERSCREEN
    };
    GameState                                   m_currentGameState;

    enum class UiState
    {
        UISTATE_SWING,
        UISTATE_SHOT,
        UISTATE_SCORE
    };
    UiState                                     m_currentUiState;

    // audio 
    std::unique_ptr<DirectX::AudioEngine>       m_audioEngine;
    bool                                        m_retryAudio;
    float                                       m_musicVolume = 0.9f;
    float                                       m_sfxVolume = 0.5f;
    std::unique_ptr<DirectX::WaveBank>          m_audioBank;
    std::unique_ptr<DirectX::SoundStreamInstance> m_audioMusicStream;
    std::unique_ptr<DirectX::SoundStreamInstance> m_audioEffectStream;

    const bool                                  m_isInDebugMode = true;

    DirectX::SimpleMath::Vector3                m_cameraFocusPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3                m_lightPos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos2 = DirectX::SimpleMath::Vector3::UnitX;

    // light pos for m_effect2
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos2 = DirectX::SimpleMath::Vector3::UnitX;

    std::vector<std::pair<DirectX::VertexPositionNormalColor, DirectX::VertexPositionNormalColor>>   m_worldGrid;
    std::vector<std::pair<DirectX::VertexPositionColor, DirectX::VertexPositionColor>>   m_debugWorldGrid;

    bool                                        m_moveLightWithCameraFocus1 = false;
    bool                                        m_moveLightWithCameraFocus2 = false;
    bool                                        m_moveLightWithCameraFocus3 = false;

    const float                                 m_lightMovementSpeed = 0.4;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundTex;

    int                                         m_testDisplayCount = 0;
    const int                                   m_testDisplayCountMax = 3;

    // for height map
    std::vector<DirectX::VertexPositionColor>   m_terrainVector;
    DirectX::VertexPositionColor* m_terrainVertexArray;
    DirectX::VertexPositionColor* m_terrainVertexArrayBase;
    int                                         m_terrainVertexCount;
    std::vector<DirectX::VertexPositionNormalColor>   m_terrainVector2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArray2;
    DirectX::VertexPositionNormalColor* m_terrainVertexArrayBase2;
    int                                  m_terrainVertexCount2;
    Terrain m_terrainGamePlay;
    Terrain m_terrainStartScreen;
    DirectX::SimpleMath::Vector4        m_startScreenGridDimmerColor = DirectX::SimpleMath::Vector4(0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f);
    float                               m_debugValue1 = 0.0;
    float                               m_debugValue2 = 0.0;
    float                               m_debugValue3 = 0.0;
    float                               m_debugValue4 = 0.0;
    float                               m_debugValue5 = 0.0;

    double                              m_testTimer = 0.0;
    double                              m_testTimerOffset = 0.0;

    // start scene control variables
    /*
    const float                         m_fadeDuration = 4.0f;
    const float                         m_logoDisplayDuration = 9.5f;
    const float                         m_logoDisplayGap = 1.0f;
    const float                         m_startDelay = 5.2f;
    */
    /*
    const float                         m_fadeDuration = 4.0f;
    const float                         m_logoDisplayDuration = 11.5f;
    const float                         m_logoDisplayGap = 0.8f;
    const float                         m_startDelay = 5.2f;
    */
    
    const float                         m_fadeDuration = 1.0f;
    const float                         m_logoDisplayDuration = 5.1f;
    const float                         m_logoDisplayGap = 0.1f;
    const float                         m_startDelay = 5.5f;
    
    
    /*
    const float                         m_fadeDuration = 1.2f;
    const float                         m_logoDisplayDuration = 17.5f;
    const float                         m_logoDisplayGap = 0.2f;
    const float                         m_startDelay = 1.2f;
    */

    const float                         m_fogGap1 = 0.0;
    const float                         m_fogGap2 = 10.0;

    DirectX::SimpleMath::Vector3        m_introCamPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3        m_introCamTarg = DirectX::SimpleMath::Vector3(1.1, 0.0, 0.0);

    DirectX::SimpleMath::Vector3        m_introCamTarg2 = DirectX::SimpleMath::Vector3(.9, -0.03, 0.0);
    DirectX::SimpleMath::Vector3        m_introCamPos2 = DirectX::SimpleMath::Vector3(0.0, -0.03, 0.0);

    DirectX::SimpleMath::Vector3        m_startScreenCamPos = DirectX::SimpleMath::Vector3(-0.8, 0.2, 0.0);
    DirectX::SimpleMath::Vector3        m_startScreenCamTarg = DirectX::SimpleMath::Vector3(1.1, 0.1, 0.0);

    DirectX::SimpleMath::Vector3        m_startScreenCamZoomPos = DirectX::SimpleMath::Vector3(4.2, 0.1, 0.0);
    DirectX::SimpleMath::Vector3        m_startScreenZCamZoomTarg = DirectX::SimpleMath::Vector3(7.3, 0.0, 0.0);                      

    const float                         m_teaserScreenDistance = 90.0;

    DirectX::SimpleMath::Vector3        m_teaserCamPos = DirectX::SimpleMath::Vector3(m_teaserScreenDistance - 1.0f, 0.0, 0.0);
    DirectX::SimpleMath::Vector3        m_teaserCamTarg = DirectX::SimpleMath::Vector3(m_teaserScreenDistance, 0.0, 0.0);
    
    /*
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos1 = DirectX::SimpleMath::Vector3(8.0f, 1.0, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg1 = DirectX::SimpleMath::Vector3(0.0, -4.5, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos2 = DirectX::SimpleMath::Vector3(12.0f, 1.0, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg2 = DirectX::SimpleMath::Vector3(0.0, 3.5, 0.0);
    */
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos1 = DirectX::SimpleMath::Vector3(8.0f, 1.0, 0.0);
    //DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg1 = DirectX::SimpleMath::Vector3(0.0, -3.8, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg1 = DirectX::SimpleMath::Vector3(0.0, -3.8, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamPos2 = DirectX::SimpleMath::Vector3(18.0f, 3.5, 0.0);
    //DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg2 = DirectX::SimpleMath::Vector3(0.0, 4.4, 0.0);
    DirectX::SimpleMath::Vector3        m_gamePlayStartCamTarg2 = DirectX::SimpleMath::Vector3(0.0, 5.0f, 0.0);
    float                               m_gamePlayStartOffSetTimer = 0.0f;

    DirectX::SimpleMath::Vector3        m_testNorm = DirectX::SimpleMath::Vector3::UnitZ;

    float                               m_terrainColorDimmer = 0.0;

    //const double                         m_debugStartTime = -5.0;
    const double                         m_debugStartTime = 0.0;
    const float                         m_startScreenTimerMod = 0.0f;
    const float                         m_startScreenTimerMod2 = 0.0f;

    float                               m_loadScreenTimerStart = 0.0f;

    DirectX::XMFLOAT4 m_testColor;
    const DirectX::XMFLOAT4 m_defaultStartTerrainColor = DirectX::XMFLOAT4(0.1f, 0.01f, 0.01f, 1.0f);
    const DirectX::XMFLOAT4 m_defaultGameTerrainColor = DirectX::XMFLOAT4(0.0f, 0.292156899f, 0.0f, 1.0f);

    const float m_startTerrainMaxY = 0.347000033f;
    const float m_gameTerrainMaxY = 2471.24097f;

    float m_testTimer1 = 0.0f;
    float m_testTimer2 = 0.0f;

    bool m_debugToggle = false;
};
