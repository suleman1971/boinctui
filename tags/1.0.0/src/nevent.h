#ifndef NEVENT_H
#define NEVENT_H


class NEvent //класс описывающий событие создаваемое например при нажатии клавиш
{
  public:
    enum 	Type 		{ evKB, evMOUSE, evPROG }; //evPROG событие генерируемое самой прораммой
    NEvent(NEvent::Type type, int keycode)	{ this->type = type; this->done = false; this->keycode = keycode;};
    NEvent::Type	type;
    bool		done;		//true если обработано
    union
    {
	int		keycode;	//код клавиатуры
	int		cmdcode;	//произвольный код команды
    };
};


#endif //NEVENT_H