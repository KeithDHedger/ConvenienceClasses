
#include "cliPrefsClass.h"

/**
* this->dialogPrefsClass class destroy.
*/
cliPrefsClass::~cliPrefsClass()
{
}

/**
* this->dialogPrefsClass.
*/
cliPrefsClass::cliPrefsClass(QString pname)
{
}

void cliPrefsClass::appendStrPref(QString name,QString str)
{
	if(this->prefsData.contains(qHash(name)))
		this->setPrefValue(name,this->getPrefValue(name)<<str);
	else
		this->prefsData[qHash(name)]=QStringList({str});
}

void cliPrefsClass::setPrefValue(QString name,QStringList val)
{
	this->prefsData[qHash(name)]=val;
}

QStringList cliPrefsClass::getPrefValue(QString name)
{
	return(this->prefsData.value(qHash(name)));
}

bool cliPrefsClass::doCliArgs(int argc,char **argv,option longoptions[])
{
	int			ocnt=0;
	int			c;
	std::string	optstr="";
	int			option_index;

	while(longoptions[ocnt].name!=0)
		{
			optstr+=longoptions[ocnt].val;
			if(longoptions[ocnt].has_arg!=no_argument)
				{
					if(longoptions[ocnt].has_arg==required_argument)
						optstr+=":";
					if(longoptions[ocnt].has_arg==optional_argument)
						optstr+="::";
				}
			ocnt++;
		}

	optstr+="?h";

	while(1)
		{
			option_index=0;
			c=getopt_long(argc,argv,optstr.c_str(),longoptions,&option_index);
			if(c==-1)
				break;
			if(c=='?' || c=='h')
				return(false);

			ocnt=0;
			while(longoptions[ocnt].name!=0)
				{
					if(longoptions[ocnt].val==c)
						{
							if(optarg!=NULL)
								{
									this->appendStrPref(longoptions[ocnt].name,QString(optarg));
								}
							else
								{
									if(longoptions[ocnt].has_arg==optional_argument)
										this->appendStrPref(longoptions[ocnt].name,QString(optarg));
									else
										this->appendStrPref(longoptions[ocnt].name,"");
								}
						}
					ocnt++;
				}			
		}

	while(optind<argc)
		{
			this->extraCliArgs<<argv[optind];
			optind++;
		}
	return(true);
}
