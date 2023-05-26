//Custom static variables

//Background image settings
const bool FadeBackground = false; // Do you want the fade effect
const string LOADING_SCREENS_PATH = "Loadingscreen/Pictures/loading_screen_1_co.edds"; //Custom background image
const string CUSTOM_TEXTURE_PATH =  "{F2CEA7E35B785FB7}Gui/textures/loading_screens/loading_screen_3_mask.edds"; //Custom overlay for fade in effect

//Settings for hints
const bool HideHints = true; // True = Yes/False = No

// Hint LoadingScreen settings
const string DISABLE_HINT_LAYOUT = "Loadingscreen/Layouts/loading_disable_hints.layout"; // Custom built layout to disable Hints
const string CUSTOM_HINTS_PATH = "Loadingscreen/Data/hints.json"; // Custom JSON of hints

// Hint Queue settings
const string DISABLE_QUEUE_HINT_LAYOUT = "Loadingscreen/Layouts/loading_queue_disable_hints.layout"; // Custom built layout to disable hints
const string DEFAULT_QUEUE_HINT_LAYOUT = "gui/layouts/dialog_queue_position.layout"; // Default Dayz layout

// Hint Loading timer screen
const string DISABLE_TIMER_HINT_LAYOUT = "Loadingscreen/Layouts/timer_disable_hints.layout";// Custom built layout to disable hints
const string DEFAULT_TIMER_HINT_LAYOUT = "gui/layouts/dialog_login_time.layout";// Default Dayz layout

//Misc settings
const string LOADING_SCREEN_LAYOUT = "gui/layouts/loading.layout"; // Custom loading layout if desired
const bool ShowLogo = false; // Corner logos to show
const float PROGRESSBAR_W = 600; // Progress bar width/legnth
const float PROGRESSBAR_H = 6; //Progress bar height/thickness


// First loading screen
modded class LoadingScreen

{
	void LoadingScreen(DayZGame game)
	{
        m_DayZGame = game;
		
        // Check for hints variables
        if (HideHints)
        {
            m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets(DISABLE_HINT_LAYOUT); //Disable hints
        }
        else
        {
            m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets(LOADING_SCREEN_LAYOUT); // Custom or default layout
        }
        
		Class.CastTo(m_ImageLogoMid, m_WidgetRoot.FindAnyWidget("ImageLogoMid"));
		Class.CastTo(m_ImageLogoCorner, m_WidgetRoot.FindAnyWidget("ImageLogoCorner"));
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		Class.CastTo(m_TextWidgetStatus, m_WidgetRoot.FindAnyWidget("StatusText"));
		Class.CastTo(m_ImageWidgetBackground, m_WidgetRoot.FindAnyWidget("ImageBackground"));
		Class.CastTo(m_ImageLoadingIcon, m_WidgetRoot.FindAnyWidget("ImageLoadingIcon"));
		Class.CastTo(m_ModdedWarning, m_WidgetRoot.FindAnyWidget("ModdedWarning"));
		
		

		m_ImageBackground = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
		m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
        string tmp;
		m_ProgressText = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("ProgressText"));
		if (GetGame())
		{
			m_ProgressText.Show(GetGame().CommandlineGetParam("loadingTest", tmp));
		}
		m_WidgetRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			Widget expNotification = m_WidgetRoot.FindAnyWidget("notification_root");
			if (expNotification)
			{
				expNotification.Show(true);
			}
		#endif
		#endif
		#endif

        // Apply variables above
        m_ProgressLoading.SetSize(PROGRESSBAR_W, PROGRESSBAR_H);
		m_ModdedWarning.Show(ShowLogo);
		m_ImageLogoMid.Show(ShowLogo);
		m_ImageLogoCorner.Show(ShowLogo);
		m_ImageBackground.LoadImageFile(0, LOADING_SCREENS_PATH);
		m_Counter = 0;
		
        // Check if we are fading in or not
        if (!FadeBackground)
		{
            m_ImageBackground.LoadMaskTexture("");
		}
        else
        {
            m_ImageBackground.LoadMaskTexture(CUSTOM_TEXTURE_PATH);
		    game.GetBacklit().LoadingAnim();
        }
		
		ProgressAsync.SetProgressData(m_ProgressLoading);
		ProgressAsync.SetUserData(m_ImageBackground);
	}

	override void Show()
	{
		Widget lIcon = m_ImageBackground;
		Widget pText = m_ProgressLoading;
		m_ProgressText.SetText("");
		m_ProgressLoading.SetCurrent(0.0);

		
		if (!m_WidgetRoot.IsVisible())
		{
			if (m_DayZGame.GetUIManager().IsDialogVisible())
			{
				m_DayZGame.GetUIManager().HideDialog();
			}
			
            // Setting the variables as intended
			m_HintPanel = null;
			m_ImageLogoMid.Show(ShowLogo);
			m_ImageLogoCorner.Show(ShowLogo);				
			m_TextWidgetStatus.Show(ShowLogo);
			m_WidgetRoot.Show(true);
			m_TextWidgetTitle.SetText("");
			m_TextWidgetStatus.SetText("");
		}
		
		ProgressAsync.SetProgressData(pText);
		ProgressAsync.SetUserData(lIcon);
	}

};

// Timer screen
modded class LoginTimeBase extends LoginScreenBase
{
	
	override Widget Init()
	{
		// Check if we disable hints again
		if (HideHints)
		{
			layoutRoot 			= GetGame().GetWorkspace().CreateWidgets(DISABLE_TIMER_HINT_LAYOUT); // Disable Hints
		}
		else
		{
			layoutRoot 			= GetGame().GetWorkspace().CreateWidgets(DEFAULT_TIMER_HINT_LAYOUT); // Enable Hints
		}
		
		
		m_txtDescription 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_txtLabel 			= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLabel"));
		m_btnLeave 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));

		m_txtDescription.Show(true);
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
		layoutRoot.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif

		return layoutRoot;
	}
		
	void Show()
	{
		if (layoutRoot)
		{
			layoutRoot.Show(true);
			// Just incase they sneak up on us
			if (HideHints)
			{
				m_HintPanel = null; // Render everying useless
			}
			else
			{
				m_HintPanel	= new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
			}
			
		}
	}
	
};

// Queue page 
modded class LoginQueueBase extends LoginScreenBase
{
	
	override Widget Init()
	{	
		//Check if we are hiding hints again
		if (HideHints)
		{
			layoutRoot 		= GetGame().GetWorkspace().CreateWidgets(DISABLE_QUEUE_HINT_LAYOUT);//Disable Hints
		}	
		else
		{
			layoutRoot 		= GetGame().GetWorkspace().CreateWidgets(DEFAULT_QUEUE_HINT_LAYOUT);// Allow Hints
		}
		
		m_HintPanel	= new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
		m_txtPosition	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtPosition"));
		m_txtNote 		= TextWidget.Cast(layoutRoot.FindAnyWidget("txtNote"));
		m_btnLeave 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
		m_txtNote.Show(true);
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
		layoutRoot.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif

		return layoutRoot;
	}
	
};


//Custom loading of hints
modded class UiHintPanel extends ScriptedWidgetEventHandler
{
	// Load content data from json file 
	override protected void LoadContentList()
	{
        //This loads the custom JSON
		JsonFileLoader<array<ref HintPage>>.JsonLoadFile( CUSTOM_HINTS_PATH, m_ContentList );
	}	
};
