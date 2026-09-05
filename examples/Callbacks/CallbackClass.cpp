/*
 *
 * ©K. D. Hedger. Sat  5 Sep 13:05:00 BST 2026 keithdhedger@gmail.com

 * This file (CallbackClass.cpp) is part of examples.

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

#include "CallbackClass.h"

CallbackClassClass::~CallbackClassClass()
{
}

CallbackClassClass::CallbackClassClass()
{
}

void CallbackClassClass::connectCB(CC_Callback cb)
{
	this->callbacks.push_back(cb);
}

void CallbackClassClass::clearCallbacks(void)
{
	this->callbacks.clear();
}

void CallbackClassClass::triggerCallbacks(QString txt)
{
	for(int j=0;j<this->callbacks.size();j++)
		this->callbacks.at(j)(txt);
}

void CallbackClassClass::triggerCallback(int number,QString txt)
{
	this->callbacks.at(number)(txt);
}
