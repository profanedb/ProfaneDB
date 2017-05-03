/*
 * ProfaneDB - A Protocol Buffer database.
 * Copyright (C) 2017  "Giorgio Azzinnaro" <giorgio.azzinnaro@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "normalizer.h"

profanedb::storage::Normalizer::Normalizer(Parser & parser)
  : schemaPool(parser.schemaPool)
  , normalizedMergedDescriptorDb(
      new google::protobuf::MergedDescriptorDatabase(parser.rootDescriptorDb.get(), parser.normalizedDescriptorDb.get()))
  , normalizedPool(new google::protobuf::DescriptorPool(normalizedMergedDescriptorDb.get()))
{
}

map< std::string, std::shared_ptr<google::protobuf::Message> > profanedb::storage::Normalizer::NormalizeMessage(
    const google::protobuf::Any & serializable)
{
    // Any messages have a type url beginning with `type.googleapis.com/`, this is stripped
    std::string type = serializable.type_url();
    auto definition = std::unique_ptr<const google::protobuf::Descriptor>(schemaPool->FindMessageTypeByName(type.substr(type.rfind('/')+1, std::string::npos)));
    
    // Having the definition our message factory can simply generate a container,
    auto container = std::unique_ptr<google::protobuf::Message>(messageFactory.GetPrototype(definition.get())->New());
    
    // and convert the bytes coming from Any into it
    serializable.UnpackTo(container.get());
    
    // The method getting a Message as paramater does the actual normalization of data
    return this->NormalizeMessage(*container);
}

map< std::string, std::shared_ptr<google::protobuf::Message> > profanedb::storage::Normalizer::NormalizeMessage(
    const google::protobuf::Message & message) const
{
}
