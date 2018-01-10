#pragma once
#include <map>
namespace Resources
{
    namespace Fonts
    {
        enum Files
        {
            KG14,
            KG18,
            KG26,
            nordic,
            nordic14,
            nordic25
        };
        const std::map<Files, const char *> Paths =
        {
            {KG14, R"(Resources\Fonts\KG14.spritefont)"},
            {KG18, R"(Resources\Fonts\KG18.spritefont)"},
            {KG26, R"(Resources\Fonts\KG26.spritefont)"},
            {nordic, R"(Resources\Fonts\nordic.spritefont)"},
            {nordic14, R"(Resources\Fonts\nordic14.spritefont)"},
            {nordic25, R"(Resources\Fonts\nordic25.spritefont)"}
        };
        
        Files toEnum(const char * str);
    }
    namespace Maps
    {
        enum Files
        {
            b,
            g,
            lights,
            lights2,
            r,
            Stock_Map
        };
        const std::map<Files, const char *> Paths =
        {
            {b, R"(Resources\Maps\b.toml)"},
            {g, R"(Resources\Maps\g.toml)"},
            {lights, R"(Resources\Maps\lights.toml)"},
            {lights2, R"(Resources\Maps\lights2.toml)"},
            {r, R"(Resources\Maps\r.toml)"},
            {Stock_Map, R"(Resources\Maps\Stock_Map.toml)"}
        };
        
        Files toEnum(const char * str);
        namespace OLD
        {
            enum Files
            {
                Stock_Map,
                Stock_Map_V6,
                Test_Map
            };
            const std::map<Files, const char *> Paths =
            {
                {Stock_Map, R"(Resources\Maps\OLD\Stock_Map.toml)"},
                {Stock_Map_V6, R"(Resources\Maps\OLD\Stock_Map_V6.toml)"},
                {Test_Map, R"(Resources\Maps\OLD\Test_Map.toml)"}
            };
            
            Files toEnum(const char * str);
        }
    }
    namespace Models
    {
        enum Files
        {
            Ammocrystal,
            AmmoPackCrossBolt,
            AnimatedSummonUnit,
            AnimationTest,
            Barrel,
            Bone,
            Border_Walls,
            Bridge,
            Bush,
            Christmas_Tree,
            Cloud,
            Crate,
            Crossbow,
            Crossbowbolt,
            Crossbowbolts,
            Dead_Tree,
            Docks,
            Fence1,
            Fence2,
            Fence_Post,
            Firepit,
            Fire_Pit,
            Grappling_Point,
            GrassG1,
            GrassG2,
            Ground,
            Ground_Old,
            Grunt,
            Gyro_Bomber,
            Hammer,
            House1,
            House2,
            House3,
            Ice,
            Icecickle,
            Jump_Pad,
            Lamp_Post,
            Log,
            Lower_Island,
            Market_Tent,
            Mountain,
            Necromancer,
            NecromancerWithAnim,
            Present1,
            Present2,
            Rock1,
            Rock2,
            Rock3,
            SkySphere,
            Small_Crate,
            Snow,
            Snow_Heap,
            Snow_Heap2,
            Spear,
            Staff,
            StaticSummon,
            Stone_Decoration,
            Stone_Decoration2,
            SummonUnitWithAnim,
            Table,
            Tablet1,
            Tee_Pee,
            Tomb_Stone,
            Totem,
            TotemWithAnimation,
            Tree,
            UnitCube
        };
        const std::map<Files, const char *> Paths =
        {
            {Ammocrystal, R"(Resources\Models\Ammocrystal.hyb)"},
            {AmmoPackCrossBolt, R"(Resources\Models\AmmoPackCrossBolt.hyb)"},
            {AnimatedSummonUnit, R"(Resources\Models\AnimatedSummonUnit.hyb)"},
            {AnimationTest, R"(Resources\Models\AnimationTest.hyb)"},
            {Barrel, R"(Resources\Models\Barrel.hyb)"},
            {Bone, R"(Resources\Models\Bone.hyb)"},
            {Border_Walls, R"(Resources\Models\Border_Walls.hyb)"},
            {Bridge, R"(Resources\Models\Bridge.hyb)"},
            {Bush, R"(Resources\Models\Bush.hyb)"},
            {Christmas_Tree, R"(Resources\Models\Christmas_Tree.hyb)"},
            {Cloud, R"(Resources\Models\Cloud.hyb)"},
            {Crate, R"(Resources\Models\Crate.hyb)"},
            {Crossbow, R"(Resources\Models\Crossbow.hyb)"},
            {Crossbowbolt, R"(Resources\Models\Crossbowbolt.hyb)"},
            {Crossbowbolts, R"(Resources\Models\Crossbowbolts.hyb)"},
            {Dead_Tree, R"(Resources\Models\Dead_Tree.hyb)"},
            {Docks, R"(Resources\Models\Docks.hyb)"},
            {Fence1, R"(Resources\Models\Fence1.hyb)"},
            {Fence2, R"(Resources\Models\Fence2.hyb)"},
            {Fence_Post, R"(Resources\Models\Fence_Post.hyb)"},
            {Firepit, R"(Resources\Models\Firepit.hyb)"},
            {Fire_Pit, R"(Resources\Models\Fire_Pit.hyb)"},
            {Grappling_Point, R"(Resources\Models\Grappling_Point.hyb)"},
            {GrassG1, R"(Resources\Models\GrassG1.hyb)"},
            {GrassG2, R"(Resources\Models\GrassG2.hyb)"},
            {Ground, R"(Resources\Models\Ground.hyb)"},
            {Ground_Old, R"(Resources\Models\Ground_Old.hyb)"},
            {Grunt, R"(Resources\Models\Grunt.hyb)"},
            {Gyro_Bomber, R"(Resources\Models\Gyro_Bomber.hyb)"},
            {Hammer, R"(Resources\Models\Hammer.hyb)"},
            {House1, R"(Resources\Models\House1.hyb)"},
            {House2, R"(Resources\Models\House2.hyb)"},
            {House3, R"(Resources\Models\House3.hyb)"},
            {Ice, R"(Resources\Models\Ice.hyb)"},
            {Icecickle, R"(Resources\Models\Icecickle.hyb)"},
            {Jump_Pad, R"(Resources\Models\Jump_Pad.hyb)"},
            {Lamp_Post, R"(Resources\Models\Lamp_Post.hyb)"},
            {Log, R"(Resources\Models\Log.hyb)"},
            {Lower_Island, R"(Resources\Models\Lower_Island.hyb)"},
            {Market_Tent, R"(Resources\Models\Market_Tent.hyb)"},
            {Mountain, R"(Resources\Models\Mountain.hyb)"},
            {Necromancer, R"(Resources\Models\Necromancer.hyb)"},
            {NecromancerWithAnim, R"(Resources\Models\NecromancerWithAnim.hyb)"},
            {Present1, R"(Resources\Models\Present1.hyb)"},
            {Present2, R"(Resources\Models\Present2.hyb)"},
            {Rock1, R"(Resources\Models\Rock1.hyb)"},
            {Rock2, R"(Resources\Models\Rock2.hyb)"},
            {Rock3, R"(Resources\Models\Rock3.hyb)"},
            {SkySphere, R"(Resources\Models\SkySphere.hyb)"},
            {Small_Crate, R"(Resources\Models\Small_Crate.hyb)"},
            {Snow, R"(Resources\Models\Snow.hyb)"},
            {Snow_Heap, R"(Resources\Models\Snow_Heap.hyb)"},
            {Snow_Heap2, R"(Resources\Models\Snow_Heap2.hyb)"},
            {Spear, R"(Resources\Models\Spear.hyb)"},
            {Staff, R"(Resources\Models\Staff.hyb)"},
            {StaticSummon, R"(Resources\Models\StaticSummon.hyb)"},
            {Stone_Decoration, R"(Resources\Models\Stone_Decoration.hyb)"},
            {Stone_Decoration2, R"(Resources\Models\Stone_Decoration2.hyb)"},
            {SummonUnitWithAnim, R"(Resources\Models\SummonUnitWithAnim.hyb)"},
            {Table, R"(Resources\Models\Table.hyb)"},
            {Tablet1, R"(Resources\Models\Tablet1.hyb)"},
            {Tee_Pee, R"(Resources\Models\Tee_Pee.hyb)"},
            {Tomb_Stone, R"(Resources\Models\Tomb_Stone.hyb)"},
            {Totem, R"(Resources\Models\Totem.hyb)"},
            {TotemWithAnimation, R"(Resources\Models\TotemWithAnimation.hyb)"},
            {Tree, R"(Resources\Models\Tree.hyb)"},
            {UnitCube, R"(Resources\Models\UnitCube.hyb)"}
        };
        
        Files toEnum(const char * str);
    }
    namespace Particles
    {
        enum Files
        {
            SPLASH
        };
        const std::map<Files, const char *> Paths =
        {
            {SPLASH, R"(Resources\Particles\SPLASH.png)"}
        };
        
        Files toEnum(const char * str);
    }
    namespace Shaders
    {
        enum Files
        {
            DebugShader,
            DoFBlur1,
            DoFBlur2,
            DoFCreateCoC,
            DoFGlue,
            ForwardPlus,
            ForwardPlus_PS,
            ForwardPlus_PS_Foliage,
            ForwardPlus_PS_Foliage_Depth,
            ForwardPlus_VS_Animated,
            ForwardPlus_VS_Foliage,
            ForwardPlus_VS_Static,
            FullscreenQuad,
            GlowBlurHorizontal,
            GlowBlurVertical,
            GlowDownSampler,
            KawaseDualFilterDownsample,
            KawaseDualFilterUpsample,
            KawaseDualFilterVS,
            LightGridCulling,
            Moon,
            ParticlePlane,
            PostFX,
            SkyShader,
            SnowShader,
            SpriteShader,
            SSAO,
            SSAOBlur,
            SSAOComputeShader,
            SSAOGaussianBlurHorizontal,
            SSAOGaussianBlurVertical,
            SSAOGenerate,
            SSAOMerger
        };
        const std::map<Files, const char *> Paths =
        {
            {DebugShader, R"(Resources\Shaders\DebugShader.hlsl)"},
            {DoFBlur1, R"(Resources\Shaders\DoFBlur1.hlsl)"},
            {DoFBlur2, R"(Resources\Shaders\DoFBlur2.hlsl)"},
            {DoFCreateCoC, R"(Resources\Shaders\DoFCreateCoC.hlsl)"},
            {DoFGlue, R"(Resources\Shaders\DoFGlue.hlsl)"},
            {ForwardPlus, R"(Resources\Shaders\ForwardPlus.hlsl)"},
            {ForwardPlus_PS, R"(Resources\Shaders\ForwardPlus_PS.hlsl)"},
            {ForwardPlus_PS_Foliage, R"(Resources\Shaders\ForwardPlus_PS_Foliage.hlsl)"},
            {ForwardPlus_PS_Foliage_Depth, R"(Resources\Shaders\ForwardPlus_PS_Foliage_Depth.hlsl)"},
            {ForwardPlus_VS_Animated, R"(Resources\Shaders\ForwardPlus_VS_Animated.hlsl)"},
            {ForwardPlus_VS_Foliage, R"(Resources\Shaders\ForwardPlus_VS_Foliage.hlsl)"},
            {ForwardPlus_VS_Static, R"(Resources\Shaders\ForwardPlus_VS_Static.hlsl)"},
            {FullscreenQuad, R"(Resources\Shaders\FullscreenQuad.hlsl)"},
            {GlowBlurHorizontal, R"(Resources\Shaders\GlowBlurHorizontal.hlsl)"},
            {GlowBlurVertical, R"(Resources\Shaders\GlowBlurVertical.hlsl)"},
            {GlowDownSampler, R"(Resources\Shaders\GlowDownSampler.hlsl)"},
            {KawaseDualFilterDownsample, R"(Resources\Shaders\KawaseDualFilterDownsample.hlsl)"},
            {KawaseDualFilterUpsample, R"(Resources\Shaders\KawaseDualFilterUpsample.hlsl)"},
            {KawaseDualFilterVS, R"(Resources\Shaders\KawaseDualFilterVS.hlsl)"},
            {LightGridCulling, R"(Resources\Shaders\LightGridCulling.hlsl)"},
            {Moon, R"(Resources\Shaders\Moon.hlsl)"},
            {ParticlePlane, R"(Resources\Shaders\ParticlePlane.hlsl)"},
            {PostFX, R"(Resources\Shaders\PostFX.hlsl)"},
            {SkyShader, R"(Resources\Shaders\SkyShader.hlsl)"},
            {SnowShader, R"(Resources\Shaders\SnowShader.hlsl)"},
            {SpriteShader, R"(Resources\Shaders\SpriteShader.hlsl)"},
            {SSAO, R"(Resources\Shaders\SSAO.hlsl)"},
            {SSAOBlur, R"(Resources\Shaders\SSAOBlur.hlsl)"},
            {SSAOComputeShader, R"(Resources\Shaders\SSAOComputeShader.hlsl)"},
            {SSAOGaussianBlurHorizontal, R"(Resources\Shaders\SSAOGaussianBlurHorizontal.hlsl)"},
            {SSAOGaussianBlurVertical, R"(Resources\Shaders\SSAOGaussianBlurVertical.hlsl)"},
            {SSAOGenerate, R"(Resources\Shaders\SSAOGenerate.hlsl)"},
            {SSAOMerger, R"(Resources\Shaders\SSAOMerger.hlsl)"}
        };
        
        Files toEnum(const char * str);
    }
    namespace Textures
    {
        enum Files
        {
            CardBackground,
            controlscheme,
            Crosshair,
            Diffusemoon,
            Enraged,
            Gameover,
            Gamesheet,
            Grid,
            Highscore,
            Highscoretext,
            iconsheet,
            IntroScreen,
            LaunchScreen,
            Loadingscreen,
            MainmenuClean,
            Mainmenutext,
            MissingTexture,
            particle,
            particle_white,
            randomNormals,
            Retrytextsheet,
            Settings,
            SettingsButtons,
            Skillpickbackground,
            Skillpicksheet,
            SnowFlake,
            Spellsheet,
            TextSkillPick,
            wavebegins,
            WaveComplete,
            weaponsheet
        };
        const std::map<Files, const char *> Paths =
        {
            {CardBackground, R"(Resources\Textures\CardBackground.dds)"},
            {controlscheme, R"(Resources\Textures\controlscheme.dds)"},
            {Crosshair, R"(Resources\Textures\Crosshair.dds)"},
            {Diffusemoon, R"(Resources\Textures\Diffusemoon.dds)"},
            {Enraged, R"(Resources\Textures\Enraged.dds)"},
            {Gameover, R"(Resources\Textures\Gameover.dds)"},
            {Gamesheet, R"(Resources\Textures\Gamesheet.dds)"},
            {Grid, R"(Resources\Textures\Grid.png)"},
            {Highscore, R"(Resources\Textures\Highscore.dds)"},
            {Highscoretext, R"(Resources\Textures\Highscoretext.dds)"},
            {iconsheet, R"(Resources\Textures\iconsheet.dds)"},
            {IntroScreen, R"(Resources\Textures\IntroScreen.dds)"},
            {LaunchScreen, R"(Resources\Textures\LaunchScreen.dds)"},
            {Loadingscreen, R"(Resources\Textures\Loadingscreen.dds)"},
            {MainmenuClean, R"(Resources\Textures\MainmenuClean.dds)"},
            {Mainmenutext, R"(Resources\Textures\Mainmenutext.dds)"},
            {MissingTexture, R"(Resources\Textures\MissingTexture.dds)"},
            {particle, R"(Resources\Textures\particle.png)"},
            {particle_white, R"(Resources\Textures\particle_white.png)"},
            {randomNormals, R"(Resources\Textures\randomNormals.png)"},
            {Retrytextsheet, R"(Resources\Textures\Retrytextsheet.dds)"},
            {Settings, R"(Resources\Textures\Settings.dds)"},
            {SettingsButtons, R"(Resources\Textures\SettingsButtons.dds)"},
            {Skillpickbackground, R"(Resources\Textures\Skillpickbackground.dds)"},
            {Skillpicksheet, R"(Resources\Textures\Skillpicksheet.dds)"},
            {SnowFlake, R"(Resources\Textures\SnowFlake.dds)"},
            {Spellsheet, R"(Resources\Textures\Spellsheet.dds)"},
            {TextSkillPick, R"(Resources\Textures\TextSkillPick.dds)"},
            {wavebegins, R"(Resources\Textures\wavebegins.dds)"},
            {WaveComplete, R"(Resources\Textures\WaveComplete.dds)"},
            {weaponsheet, R"(Resources\Textures\weaponsheet.dds)"}
        };
        
        Files toEnum(const char * str);
    }
}
