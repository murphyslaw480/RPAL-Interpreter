#include <string>
#include <queue>
#include <boost/tokenizer.hpp>

using namespace std;

class RpalScanner
{
  static const int N_KEYWORDS;
  static const string KEYWORDS[];
  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;

  public:
  RpalScanner(char* filename, bool showSource);
  string Next_Token();
  bool Has_Next();

  static bool IsID(string token);
  static bool IsString(string token);
  static bool IsInt(string token);
  
  private:
  void process_tokens(Tokenizer tokenizer);
  queue<string> _tokens;
};
