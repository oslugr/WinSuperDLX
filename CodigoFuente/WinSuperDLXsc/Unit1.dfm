�
 TFORMPRINCIPAL 0�  TPF0TFormPrincipalFormPrincipalLeftGTop� WidthHeight1BorderIconsbiSystemMenu
biMinimize CaptionWinSuperDLXColor	clBtnFaceFont.CharsetDEFAULT_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameMS Sans Serif
Font.Style 	Icon.Data
�             �     (       @         �                        �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� """"������������""""������������""""������������""""������������""""������������""""������������""""������������""""������������"����  ��   ���/""/���������/""/��������"""/����������""/����������"��"���������/"""����������/""/��������/""/���������"����  ��������""""������������""""������������""""������������""""������������""""������������""""������������"�""������������"̻"������������"��"������������"��"������������""�"������������""""������������""""������������""""������������                                                                                                                                Menu	MainMenu1OldCreateOrderPositionpoScreenCenterShowHint	OnCreate
FormCreatePixelsPerInch`
TextHeight TLabelLabel1LeftTop#WidthpHeightCaptionINT Instruction Window  TLabelLabel2LeftTop� WidthuHeightCaptionFloat Instruction Window  TLabelLabel3LeftTop3Width�HeightCaptiond#  Opcode   address             rb op1                    ok1  typ1 op2              ok2  typ2  pred  TLabelLabel4LeftTop� Width�HeightCaptiond#  Opcode   address             rb  op1                   ok1  typ1  op2             ok2  typ2  pred  TLabelLabel5Left�Top#Width]HeightCaptionINT Reorder Buffer   TLabelLabel6Left�Top� Width_HeightCaptionFloat Reorder Buffer  TLabelLabel7Left�Top3Width6HeightCaptionN#    opcode   icount  reg#        unit        result          ok  ready   flsh  TLabelLabel8Left�Top� Width6HeightCaptionN#    opcode   icount  reg#        unit        result          ok  ready   flsh  TLabelLabel9LeftTopWidthTHeightCaptionInstruction Queue  TLabelLabel10LeftTopWidth7HeightCaptionLoad BufferTransparent	  TLabelLabel11Left�Top�Width8HeightCaptionStore Buffer  TLabelLabel12LeftTopWidth� HeightCaption/#    opcode   icount     address    rel    flsh  TLabelLabel13Left�Top�Width%HeightCaptionD#  opcode  icount   address  data to store  type   ok    rel    flsh  TLabelLabel14LeftTop$WidthHeightCaptionC#  icount       address             opcode   rs1  rs2   rd    extra  
TStatusBarStBarLeft Top�WidthHeightAutoHint	Panels ParentShowHintShowHint	SimplePanel	  	CTListBoxCTListVentanaIntLeftTopCWidth�HeightIFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListVentanaIntDblClick  	CTListBoxCTListVentanaFloatLeftTop� Width�HeightIFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListVentanaFloatDblClick  	CTListBoxCTListColaInsLeftTop4Width)Height� Font.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListColaInsDblClick  	CTListBoxCTListRBIntLeft�TopCWidthYHeightIFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListRBIntDblClick  	CTListBoxCTListRBFloatLeft�Top� WidthYHeightIFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListRBFloatDblClick  TToolBarToolBar1Left Top WidthHeightCaptionToolBar1TabOrderTransparent	 TBitBtnBitBtnGoLeft TopWidthIHeightCaptionGoTabOrder OnClickBitBtnGoClickStylebsNew  TButtonButton2LeftITopWidth`HeightCaptionNext Single CycleTabOrderOnClick
Next1Click  TButtonBtnNextSingleInstructionLeft� TopWidthpHeightCaptionNext Single InstructionTabOrderOnClickBtnNextSingleInstructionClick   	CTListBox
CTListLoadLeft
Top,WidthHeightAFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListLoadDblClick  	CTListBoxCTListStoreLeft�Top�Width9HeightAFont.CharsetANSI_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameCourier New
Font.Style 
ItemHeight
ParentFontTabOrder
OnDblClickCTListStoreDblClick  	TMainMenu	MainMenu1LeftZTop� 	TMenuItemFile1Caption&File 	TMenuItemLoad1Caption&LoadHint| Load DLX fileShortCutL@OnClick
Load1Click  	TMenuItemReset1Caption&ResetHint|Reset MachineOnClickReset1Click  	TMenuItemN1Caption-  	TMenuItemExit1CaptionE&xitHint|Exit from simulatorShortCutX@OnClick
Exit1Click   	TMenuItemSimulation1Caption&Simulation 	TMenuItemGo1Caption&GoShortCutxOnClickBitBtnGoClick  	TMenuItemMenuGoToCaptionGo toOnClickMenuGoToClick  	TMenuItemNext1CaptionNext Single &CycleShortCutvOnClick
Next1Click  	TMenuItemNextSingleInstruction1CaptionNext Single &InstructionShortCutwOnClickBtnNextSingleInstructionClick  	TMenuItemNext2CaptionNext ...OnClick
Next2Click   	TMenuItemStatistics2Caption
Statistics 	TMenuItemStatisticsViewCaptionView StatisticsOnClickStatisticsViewClick  	TMenuItemStatisticsSaveCaptionSave to ...OnClickStatisticsSaveClick   	TMenuItemStatistics1CaptionS&tatus 	TMenuItemROB1CaptionInteger ROBOnClickCTListRBIntDblClick  	TMenuItem	FloatROB1Caption	Float ROBOnClickCTListRBFloatDblClick  	TMenuItemN5Caption-  	TMenuItemInstructionWindow1CaptionInteger Instruction  WindowOnClickCTListVentanaIntDblClick  	TMenuItemFloatInstructionWindow1CaptionFloat Instruction  WindowOnClickCTListVentanaFloatDblClick  	TMenuItemN6Caption-  	TMenuItemInstructionQueue1CaptionInstruction &QueueOnClickCTListColaInsDblClick  	TMenuItemN7Caption-  	TMenuItemLoadStoreBuffer1CaptionLoad BufferOnClickCTListLoadDblClick  	TMenuItemStoreBuffer1CaptionStore BufferOnClickCTListStoreDblClick   	TMenuItemConfiguration1CaptionConfiguration 	TMenuItemMachineconfiguration1CaptionMachine configurationOnClickMachineconfiguration1Click  	TMenuItemN3Caption-  	TMenuItemBranchPrediction1CaptionEnable Jump PredictionOnClickBranchPrediction1Click  	TMenuItemJumpPredictionConfiguration1CaptionJump Prediction ConfigurationEnabledOnClick!JumpPredictionConfiguration1Click   	TMenuItemView1CaptionView 	TMenuItemSourcecode1CaptionSource codeOnClickSourcecode1Click  	TMenuItemRegisterFile1CaptionRegister FileOnClickRegisterFile1Click  	TMenuItemClockCycleDiagram1CaptionClock Cycle DiagramOnClickClockCycleDiagram1Click   	TMenuItemHelp1Caption&Help 	TMenuItemContens1CaptionContentsOnClickContens1Click  	TMenuItemN4Caption-  	TMenuItemAbout1CaptionAboutOnClickAbout1Click    TOpenDialogOpenDlgAbrirFicheroFilterDLX file|*.s|All|*.*
InitialDir.OptionsofHideReadOnlyofExtensionDifferentofPathMustExistofFileMustExistofEnableSizing Title	Open FileLeftZTop^  TSaveDialogSaveDialogSaveStatsLeftRTop$   