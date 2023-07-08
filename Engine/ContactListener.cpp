#include "ContactListener.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	const b2Body* bodyPtrs[] = { contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody() };
	if (bodyPtrs[0]->GetType() == b2BodyType::b2_dynamicBody &&
		bodyPtrs[1]->GetType() == b2BodyType::b2_dynamicBody)
	{
		Box* boxPtrs[] = {
			reinterpret_cast<Box*>(bodyPtrs[0]->GetUserData()),
			reinterpret_cast<Box*>(bodyPtrs[1]->GetUserData())
		};
		auto& tid0 = typeid(boxPtrs[0]->GetColorTrait());
		auto& tid1 = typeid(boxPtrs[1]->GetColorTrait());

		Color col0 = boxPtrs[0]->GetColorTrait().GetColor();
		Color col1 = boxPtrs[1]->GetColorTrait().GetColor();

		if (col0 == Colors::Red || col1 == Colors::Red)
			contactPatternMap.insert_or_assign(contact, Pattern::Remove);
		else if (col0 == Colors::Blue || col1 == Colors::Blue)
			contactPatternMap.insert_or_assign(contact, Pattern::Wololo);
		else if (col0 == Colors::White || col1 == Colors::White)
			contactPatternMap.insert_or_assign(contact, Pattern::Split);

		std::stringstream msg;
		msg << "Collision between " << tid0.name() << " and " << tid1.name() << std::endl;
		OutputDebugStringA(msg.str().c_str());
	}
}

void ContactListener::EndContact(b2Contact* contact)
{

}