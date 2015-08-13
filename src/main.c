#include <userint.h>
#include <stdio.h>

#include "../assets/frontend.h"
#include "statemachine.h"  

// Ajout d'un commentaire pour voir si le commit marche bien
// Un autre commentaire sur la machne Win10 où posh-git ne montre pas [master] dans powershell

// ----------------------------------------------------------------------------
#define MAX_LINE_LEN          1024          
#define PARSER_ERROR_MSG_LEN  MAX_PATHNAME_LEN + 100

//-----------------------------------------------------------------------------
static int Parse(const char SourceFilename[], int *CommentCount, char ErrorMessage[]);

//-----------------------------------------------------------------------------
int main(){
  
  if (InitCVIRTE(0, 0, 0) == 0) return -1;	/* out of memory */
  int hPanel = LoadPanel (0, "../../assets/frontend.uir", PANEL);
  if (hPanel<0) return -1;
  
  DisplayPanel (hPanel);
  RunUserInterface ();
  DiscardPanel (hPanel);
  return 0;
}

//-----------------------------------------------------------------------------
int CVICALLBACK QuitCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) {

  switch (event) {
    case EVENT_COMMIT:
      QuitUserInterface (0);
    break;
  }
  return 0;
}

//-----------------------------------------------------------------------------
int CVICALLBACK OnSelect (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) {
  
  static char SourceFilename[MAX_PATHNAME_LEN];
  int  Err;

  switch (event) {
    case EVENT_COMMIT:
      Err = FileSelectPopup ("../src", "*.c", "*.c;*.h", "Select a file", VAL_SELECT_BUTTON, 0, 0, 1, 0, SourceFilename);

      if(Err == VAL_EXISTING_FILE_SELECTED){
        static char InfoLine[1024];      
        sprintf(InfoLine, "\nFile to analyze : %s", SourceFilename); 
        InsertTextBoxLine (panel, PANEL_TEXTBOX, -1, InfoLine);
        
        int         CommentCount;   
        static char ErrorMessage[PARSER_ERROR_MSG_LEN+1];     
        if (Parse(SourceFilename, &CommentCount, ErrorMessage)){
          sprintf(InfoLine, "# of bytes in comments : %d", CommentCount); 
          InsertTextBoxLine (panel, PANEL_TEXTBOX, -1, InfoLine);
        }else{
          InsertTextBoxLine (panel, PANEL_TEXTBOX, -1, ErrorMessage);
        }
        
        int TotalNbLines, VisibleLines;       
        GetNumTextBoxLines (panel, PANEL_TEXTBOX, &TotalNbLines);
        GetCtrlAttribute (panel, PANEL_TEXTBOX, ATTR_VISIBLE_LINES, &VisibleLines);
        SetCtrlAttribute (panel, PANEL_TEXTBOX, ATTR_FIRST_VISIBLE_LINE, TotalNbLines-VisibleLines);
      }
    break;
  }
  return 0;
}

//-----------------------------------------------------------------------------
static int Parse(const char SourceFilename[], int *CommentCount, char ErrorMessage[]){

  // stay in a loop reading lines from the source file
  FILE *hFile = fopen(SourceFilename, "r");
  if (hFile!=-1){
    
    // just to make sure we start to count from 0
    *CommentCount = 0;

    // stay in a loop reading characters from the line
    static char   line[MAX_LINE_LEN+1];     
    
    char *NoErr = fgets (line, sizeof(line)-1, hFile);
    while(NoErr){
      KEY_STATE     ks      = S0;        
      //unsigned int LineNo        = 0;  
      unsigned int Column = 0;
      while (line[Column]){
        // given the current state, and this input, 
        // what's the next state?
        KEY_STATE ks_next = GetState(ks, line[Column]);

        //-----------------------------------------------------------
        // Requirement 1:
        // "Any input value that starts you in S2 and keeps you in S2
        // should be counted as a comment byte."
        if ((S2 == ks_next) && (S2 == ks)) *CommentCount= *CommentCount +1;

        //-----------------------------------------------------------
        // Requirement 2:
        // "Any input value that takes you from S2 to S3 should not 
        // be counted (yet)."

        //-----------------------------------------------------------
        // Requirement 3:
        // "Any input value that takes you from S3 back to S2 should
        // be counted twice (the char to S3 plus the char back to S2)."
        if ((S2 == ks_next) && (S3 == ks)) *CommentCount = *CommentCount + 2;

        //-----------------------------------------------------------
        // Requirement 4:
        // "Any input value that starts you in S3 and keeps you in S3
        // should be counted as a comment byte." (e.g. "/* ... ***/"):
        if ((S3 == ks_next) && (S3 == ks)) *CommentCount= *CommentCount +1;

        // enter the next state:
        ks = ks_next;
        Column++;
      }
      NoErr = fgets (line, sizeof(line)-1, hFile);   
    }
    fclose(hFile);
    return 1;
  }else{
    sprintf(ErrorMessage, "Error accessing file '%s'.", SourceFilename);
    return 0;
  }
}
