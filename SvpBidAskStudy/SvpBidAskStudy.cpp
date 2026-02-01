#include "sierrachart.h"
#include <vector>

SCDLLName("SvpBidAskStudy")

SCSFExport scsf_SvpBidAskStudy(SCStudyGraphRef sg)
{
  SCSubgraphRef MinimumGraphAsk = sg.Subgraph[0];
  SCSubgraphRef MinimumGraphBid = sg.Subgraph[1];
  SCSubgraphRef BarAsk = sg.Subgraph[4];
  SCSubgraphRef BarBid = sg.Subgraph[5];
  SCSubgraphRef AskAverage = sg.Subgraph[6];
  SCSubgraphRef BidAverage = sg.Subgraph[7];
  SCSubgraphRef MarkBarAsk = sg.Subgraph[8];
  SCSubgraphRef MarkBarBid = sg.Subgraph[9];
  SCSubgraphRef Price = sg.Subgraph[10];
  SCSubgraphRef AskMA_Internal = sg.Subgraph[11];
  SCSubgraphRef BidMA_Internal = sg.Subgraph[12];

  SCInputRef MultiplikatorCross = sg.Input[0];
  SCInputRef MA = sg.Input[1];
  SCInputRef MinimumInputRefAsk = sg.Input[2];
  SCInputRef MinimumInputRefBid = sg.Input[3];
  SCInputRef PlaySoundWhenSignal = sg.Input[4];
  SCInputRef AlertSoundNumber = sg.Input[5];
  SCInputRef Stock = sg.Input[6];

  if (sg.SetDefaults)
  {
    MultiplikatorCross.SetInt(20);
    MultiplikatorCross.Name = "Multiplicator Cross";
    MA.SetInt(360);
    MA.Name = "MA";
    MinimumInputRefAsk.SetInt(80);
    MinimumInputRefAsk.Name = "Minimum Ask";
    MinimumInputRefBid.SetInt(80);
    MinimumInputRefBid.Name = "Minimum Bid";
    PlaySoundWhenSignal.SetYesNo(true);
    PlaySoundWhenSignal.Name = "Play Sound When Signal";
    AlertSoundNumber.SetInt(1);
    AlertSoundNumber.Name = "Alert Sound Number";
    Stock.SetCustomInputStrings("By params;NQ;YM;RTY");
    Stock.SetCustomInputIndex(0);
    Stock.Name = "Parameters By Stock";

    sg.GraphName = "SVP Bid Ask Study";
    sg.StudyDescription = "SVP Bid Ask Study.";

    sg.FreeDLL = 1;
    sg.AutoLoop = 1;

    BarAsk.Name = "Ask";
    BarAsk.DrawStyle = DRAWSTYLE_BAR;
    BarAsk.PrimaryColor = RGB(0, 255, 0);

    BarBid.Name = "Bid";
    BarBid.DrawStyle = DRAWSTYLE_BAR;
    BarBid.PrimaryColor = RGB(255, 0, 0);

    AskAverage.Name = "AskAvr";
    AskAverage.DrawStyle = DRAWSTYLE_IGNORE;
    AskAverage.PrimaryColor = RGB(0, 255, 0);

    BidAverage.Name = "BidAvr";
    BidAverage.DrawStyle = DRAWSTYLE_IGNORE;
    BidAverage.PrimaryColor = RGB(255, 0, 0);

    MarkBarAsk.Name = "MarkBarAsk";
    MarkBarAsk.DrawStyle = DRAWSTYLE_STAR;
    MarkBarAsk.PrimaryColor = RGB(255, 255, 255);
    MarkBarAsk.LineWidth = 11;

    MarkBarBid.Name = "MarkBarBid";
    MarkBarBid.DrawStyle = DRAWSTYLE_STAR;
    MarkBarBid.PrimaryColor = RGB(255, 255, 255);
    MarkBarBid.LineWidth = 11;

    MinimumGraphAsk.Name = "MinAsk";
    MinimumGraphAsk.DrawStyle = DRAWSTYLE_HIDDEN;
    MinimumGraphAsk.PrimaryColor = RGB(255, 255, 255);

    MinimumGraphBid.Name = "MinBid";
    MinimumGraphBid.DrawStyle = DRAWSTYLE_HIDDEN;
    MinimumGraphBid.PrimaryColor = RGB(255, 255, 255);

    Price.Name = "Price";
    Price.DrawStyle = DRAWSTYLE_IGNORE;

    AskMA_Internal.Name = "Internal Ask MA";
    AskMA_Internal.DrawStyle = DRAWSTYLE_IGNORE;

    BidMA_Internal.Name = "Internal Bid MA";
    BidMA_Internal.DrawStyle = DRAWSTYLE_IGNORE;

    Price.PrimaryColor = RGB(255, 255, 255);

    return;
  }

  if (Stock.GetIndex() == 1)
  {
    // NQ
    MultiplikatorCross.SetInt(20);
    MinimumInputRefAsk.SetInt(80);
    MinimumInputRefBid.SetInt(80);
  }
  if (Stock.GetIndex() == 2)
  {
    // YM
    MultiplikatorCross.SetInt(20);
    MinimumInputRefAsk.SetInt(50);
    MinimumInputRefBid.SetInt(50);
  }
  if (Stock.GetIndex() == 3)
  {
    // RTY
    MultiplikatorCross.SetInt(20);
    MinimumInputRefAsk.SetInt(60);
    MinimumInputRefBid.SetInt(60);
  }

  SCDateTime DayStartDateTime = sg.GetTradingDayStartDateTimeOfBar(sg.BaseDateTimeIn[sg.Index]);
  int StartOfDayIndex = sg.GetContainingIndexForSCDateTime(sg.ChartNumber, DayStartDateTime);

  float DailyHigh = 0;
  float DailyLow = 10000000;
  int StartOfDayIndexCalc = sg.Index - 50 > StartOfDayIndex ? sg.Index - 50 : StartOfDayIndex;
  for (int i = StartOfDayIndexCalc; i < sg.Index; i++)
  {
    if (sg.BaseDataIn[SC_LAST][i] >= DailyHigh)
      DailyHigh = sg.BaseDataIn[SC_LAST][i];
    if (sg.BaseDataIn[SC_LAST][i] <= DailyLow)
      DailyLow = sg.BaseDataIn[SC_LAST][i];
  }

  BarAsk[sg.Index] = sg.BaseDataIn[SC_ASKVOL][sg.Index];
  BarAsk.DataColor[sg.Index] = BarAsk.PrimaryColor;
  BarBid[sg.Index] = -sg.BaseDataIn[SC_BIDVOL][sg.Index];
  BarBid.DataColor[sg.Index] = BarBid.PrimaryColor;

  sg.MovingAverage(sg.BaseDataIn[SC_ASKVOL], AskMA_Internal, MOVAVGTYPE_WEIGHTED, MA.IntValue);
  sg.MovingAverage(sg.BaseDataIn[SC_BIDVOL], BidMA_Internal, MOVAVGTYPE_WEIGHTED, MA.IntValue);

  int previousIndex = sg.Index > 0 ? sg.Index - 1 : sg.Index;

  AskAverage[sg.Index] = AskMA_Internal[previousIndex] * MultiplikatorCross.IntValue;
  AskAverage.DataColor[sg.Index] = AskAverage.PrimaryColor;

  BidAverage[sg.Index] = -BidMA_Internal[previousIndex] * MultiplikatorCross.IntValue;
  BidAverage.DataColor[sg.Index] = BidAverage.PrimaryColor;

  float MinimumAsk = (float)MinimumInputRefAsk.IntValue;
  MinimumGraphAsk[sg.Index] = MinimumAsk;

  float MinimumBid = (float)MinimumInputRefBid.IntValue;
  MinimumGraphBid[sg.Index] = -MinimumBid;

  const int REGION = sg.GraphRegion;

  // --- ASK SIGNAL + TEXT ---
  bool AskSignal = (AskMA_Internal[previousIndex] * MultiplikatorCross.IntValue < sg.BaseDataIn[SC_ASKVOL][sg.Index])
    && (MinimumAsk <= sg.BaseDataIn[SC_ASKVOL][sg.Index]);

  if (AskSignal)
  {
    float askLevel = AskMA_Internal[previousIndex] * MultiplikatorCross.IntValue;
    MarkBarAsk[sg.Index] = askLevel;

    int lineNumberAsk = 1000000 + sg.Index;

    s_UseTool Tool;
    Tool.Clear();
    Tool.ChartNumber = sg.ChartNumber;
    Tool.DrawingType = DRAWING_TEXT;
    Tool.Region = sg.GraphRegion;
    Tool.AddMethod = UTAM_ADD_OR_ADJUST;
    Tool.LineNumber = lineNumberAsk;

    Tool.BeginIndex = sg.Index + 3;          // malý posun doprava [web:1]
    Tool.BeginValue = askLevel;

    Tool.Color = RGB(255, 255, 255);
    Tool.FontSize = 10;
    Tool.FontBold = 1;
    Tool.TransparentLabelBackground = 1;     // průhledné pozadí [web:29]

    Tool.Text.Format("%.0f", (float)sg.BaseDataIn[SC_ASKVOL][sg.Index]);

    Tool.TextAlignment = DT_LEFT | DT_VCENTER;

    sg.UseTool(Tool);

    if (PlaySoundWhenSignal.BooleanValue && sg.DownloadingHistoricalData == 0 && (sg.Index > sg.ArraySize - 5))
      sg.PlaySound(AlertSoundNumber.IntValue);
  }
  else
  {
    MarkBarAsk[sg.Index] = 0;
  }

  // --- BID SIGNAL + TEXT ---
  bool BidSignal = (-BidMA_Internal[previousIndex] * MultiplikatorCross.IntValue > -sg.BaseDataIn[SC_BIDVOL][sg.Index])
    && (MinimumBid <= sg.BaseDataIn[SC_BIDVOL][sg.Index]);

  if (BidSignal)
  {
    float bidLevel = -BidMA_Internal[previousIndex] * MultiplikatorCross.IntValue;
    MarkBarBid[sg.Index] = bidLevel;

    int lineNumberBid = 2000000 + sg.Index;

    s_UseTool Tool;
    Tool.Clear();
    Tool.ChartNumber = sg.ChartNumber;
    Tool.DrawingType = DRAWING_TEXT;
    Tool.Region = sg.GraphRegion;
    Tool.AddMethod = UTAM_ADD_OR_ADJUST;
    Tool.LineNumber = lineNumberBid;

    Tool.BeginIndex = sg.Index + 3;
    Tool.BeginValue = bidLevel;

    Tool.Color = RGB(255, 255, 255);
    Tool.FontSize = 10;
    Tool.FontBold = 1;
    Tool.TransparentLabelBackground = 1;     // průhledné pozadí [web:29]

    Tool.Text.Format("%.0f", (float)sg.BaseDataIn[SC_BIDVOL][sg.Index]);

    Tool.TextAlignment = DT_LEFT | DT_VCENTER;

    sg.UseTool(Tool);

    if (PlaySoundWhenSignal.BooleanValue && sg.DownloadingHistoricalData == 0 && (sg.Index > sg.ArraySize - 5))
      sg.PlaySound(AlertSoundNumber.IntValue);
  }
  else
  {
    MarkBarBid[sg.Index] = 0;
  }

  float DailyRange = DailyHigh - DailyLow;
  float BidAskRange = MinimumAsk + MinimumBid;
  Price[sg.Index] = 100 * ((sg.BaseDataIn[SC_LAST][sg.Index] - DailyLow) - DailyRange / 2);
}
