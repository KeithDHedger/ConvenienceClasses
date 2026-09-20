/*
 *
 * ©K. D. Hedger. Sat  5 Sep 13:05:00 BST 2026 keithdhedger@gmail.com

 * This file (CallbackClass.h) is part of examples.

 * examples is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * examples is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with examples.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CALLBACKCLASS_
#define _CALLBACKCLASS_

#include <QtWidgets>
#include <unistd.h>

//#include <functional>

class CallbackClassClass
{
	using CC_Callback=std::function<void(QString)>;

	public:
		CallbackClassClass();
		~CallbackClassClass();

		void						connectCB(CC_Callback cb);
		void						clearCallbacks(void);
		void						triggerCallbacks(QString txt);
		void						triggerCallback(int number,QString txt);

//test internal
		int						cnt=0;
		void						connectInternalCB(CC_Callback cb);
		void						testClassCB(void);

//useing qmap with hash
		void						connectCBByName(QStringView name,CC_Callback cb);
		void						triggerCallbackByName(QStringView name,QString txt);

	private:
		CC_Callback				ccCB=nullptr;
		QVector<CC_Callback>		callbacks;
		QMap<int,CC_Callback>	cbByName;

};

#endif
