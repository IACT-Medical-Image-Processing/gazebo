/*
 * Copyright 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "gazebo/transport/Node.hh"
#include "gazebo/transport/Publisher.hh"

#include "gazebo/common/Time.hh"

#include "gazebo/physics/World.hh"
#include "gazebo/physics/Collision.hh"
#include "gazebo/physics/Contact.hh"
#include "gazebo/physics/ContactManager.hh"

using namespace gazebo;
using namespace physics;

/////////////////////////////////////////////////
ContactManager::ContactManager()
{
  this->contactIndex = 0;
}

/////////////////////////////////////////////////
ContactManager::~ContactManager()
{
  this->Clear();
  this->node.reset();
  this->contactPub.reset();

  for (unsigned int i = 0 ; i < this->contactPublishers.size(); ++i)
  {
    this->contactPublishers[i]->collisions.clear();
    this->contactPublishers[i]->publisher.reset();
  }
}

/////////////////////////////////////////////////
void ContactManager::Init(WorldPtr _world)
{
  this->world = _world;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(this->world->GetName());

  this->contactPub =
    this->node->Advertise<msgs::Contacts>("~/physics/contacts");
}

/////////////////////////////////////////////////
Contact *ContactManager::NewContact(Collision *_collision1,
                                    Collision *_collision2,
                                    const common::Time &_time)
{
  Contact *result = NULL;

  if (!_collision1 || !_collision2)
    return result;

  // If no one is listening, then don't create any contact information.
  // This is a signal to the Physics engine that it can skip the extra
  // processing necessary to get back contact information.

  ContactPublisher *contactPublisher = NULL;

  //boost::unordered_map<std::string, boost::unordered_set<std::string>:: iter;
  std::vector<ContactPublisher *>::iterator iter;
//  for (iter = this->contactPublishers.begin(); iter != this->contactPublishers.end(); ++iter)
  for (unsigned int i = 0; i < this->contactPublishers.size(); ++i)
  {
//      if (iter->second.count(collision1->GetScopedName() != 0 ||
//          iter->second.count(collision2->GetScopedName() != 0)
    if (this->contactPublishers[i]->collisions.count(
        _collision1->GetScopedName()) != 0 ||
        this->contactPublishers[i]->collisions.count(
        _collision2->GetScopedName()) != 0)
    {
//      gzerr << _collision1->GetScopedName() << " " << _collision2->GetScopedName() << std::endl;
      contactPublisher = this->contactPublishers[i];
      break;
    }
  }

  if (this->contactPub->HasConnections() || contactPublisher)
  {
    // Get or create a contact feedback object.
    if (this->contactIndex < this->contacts.size())
      result = this->contacts[this->contactIndex++];
    else
    {
      result = new Contact();
      this->contacts.push_back(result);
      this->contactIndex = this->contacts.size();
    }
    if (contactPublisher)
      contactPublisher->contacts.push_back(result);
  }

  if (!result)
    return result;

  result->count = 0;
  result->collision1 = _collision1->GetScopedName();
  result->collision2 = _collision2->GetScopedName();
  result->collisionPtr1 = _collision1;
  result->collisionPtr2 = _collision2;
  result->time = _time;
  result->world = this->world;

  return result;
}

/////////////////////////////////////////////////
unsigned int ContactManager::GetContactCount() const
{
  return this->contactIndex;
}

/////////////////////////////////////////////////
Contact *ContactManager::GetContact(unsigned int _index) const
{
  if (_index < this->contactIndex)
    return this->contacts[_index];
  else
    return NULL;
}

/////////////////////////////////////////////////
const std::vector<Contact*> &ContactManager::GetContacts() const
{
  return this->contacts;
}

/////////////////////////////////////////////////
void ContactManager::ResetCount()
{
  this->contactIndex = 0;
}

/////////////////////////////////////////////////
void ContactManager::Clear()
{
  // Delete all the contacts.
  for (unsigned int i = 0; i < this->contacts.size(); ++i)
    delete this->contacts[i];

  this->contacts.clear();

  for (unsigned int i = 0 ; i < this->contactPublishers.size(); ++i)
    this->contactPublishers[i]->contacts.clear();

  // Reset the contact count to zero.
  this->contactIndex = 0;
}

/////////////////////////////////////////////////
void ContactManager::PublishContacts()
{
//  if (this->contacts.size() == 0)
//    return;

  if (!this->contactPub)
  {
    gzerr << "ContactManager has not been initialized. "
          << "Unable to publish contacts.\n";
    return;
  }

  // publish to ~/physics/contacts
  msgs::Contacts msg;
  for (unsigned int i = 0; i < this->contactIndex; ++i)
  {
    if (this->contacts[i]->count == 0)
      continue;

    msgs::Contact *contactMsg = msg.add_contact();
    this->contacts[i]->FillMsg(*contactMsg);
  }

  msgs::Set(msg.mutable_time(), this->world->GetSimTime());
  this->contactPub->Publish(msg);

  // publish to other custom topics
  msgs::Contacts msg2;
  for (unsigned int i = 0; i < this->contactPublishers.size(); ++i)
  {
    ContactPublisher *contactPublisher = this->contactPublishers[i];
    for (unsigned int j = 0; j < this->contactPublishers[i]->contacts.size();
        ++j)
    {
      if (contactPublisher->contacts[j]->count == 0)
        continue;

      msgs::Contact *contactMsg = msg2.add_contact();
      contactPublisher->contacts[j]->FillMsg(*contactMsg);
    }
    msgs::Set(msg2.mutable_time(), this->world->GetSimTime());
    contactPublisher->publisher->Publish(msg);
    contactPublisher->contacts.clear();
  }
}

/////////////////////////////////////////////////
void ContactManager::CreateFilter(const std::string &_topic,
    std::vector<std::string> _collisions)
{
  if (_collisions.empty())
    return;

  transport::PublisherPtr pub =
    this->node->Advertise<msgs::Contacts>("~/" + _topic + "/contacts");

  ContactPublisher *contactPublisher = new ContactPublisher;
  contactPublisher->topic = _topic;
  contactPublisher->publisher = pub;
  for (unsigned int i = 0; i < _collisions.size(); ++i)
    contactPublisher->collisions.insert(_collisions[i]);

  this->contactPublishers.push_back(contactPublisher);
}
