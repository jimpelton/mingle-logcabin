/* Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <cinttypes>
#include <string>
#include <vector>

#include "build/Protocol/Raft.pb.h"
#include "build/Server/RaftLogMetadata.pb.h"

#ifndef LOGCABIN_SERVER_RAFTLOG_H
#define LOGCABIN_SERVER_RAFTLOG_H

namespace LogCabin {
namespace Server {

// forward declaration
class Globals;

namespace RaftConsensusInternal {

class Log {
  public:

      // TODO(ongaro): Is there any point to this vs using
      // Protocol::Raft::Entry directly?
    struct Entry {
        Entry();
        // TODO(ongaro): client serial number
        uint64_t entryId;
        uint64_t term;
        Protocol::Raft::EntryType type;
        std::string data;
        Protocol::Raft::Configuration configuration;
    };

    explicit Log(const std::string& path = "");

    /**
     * Append a new entry to the log.
     * \param entry
     *      Its entryId is ignored; a new one is assigned and returned.
     * \return
     *      The newly appended entry's entryId.
     */
    uint64_t append(const Entry& entry);

    /**
     * Get the entry ID of the earliest entry with the same term as the last
     * log entry.
     *      The entry ID of the first entry in the log's last term, or 0 if the
     *      log is empty.
     */
    uint64_t getBeginLastTermId() const;

    /**
     * Look up an entry by ID.
     * \param entryId
     *      Must be in the range [1, getLastLogId()].
     * \return
     *      The entry corresponding to that entry ID.
     */
    const Entry& getEntry(uint64_t entryId) const;

    /**
     * Get the entry ID of the most recent entry in the log.
     * \return
     *      The entry ID of the most recent entry in the log,
     *      or 0 if the log is empty.
     */
    uint64_t getLastLogId() const;

    /**
     * Get the term of an entry in the log.
     * \param entryId
     *      Any entry ID, including 0 and those past the end of the log.
     * \return
     *      The term of the given entry in the log if it exists,
     *      or 0 otherwise.
     */
    uint64_t getTerm(uint64_t entryId) const;

    /**
     * Delete the log entries past the given entry ID.
     * \param lastEntryId
     *      After this call, the log will contain no entries with ID greater
     *      than lastEntryId. This can be any entry ID, including 0 and those
     *      past the end of the log.
     */
    void truncate(uint64_t lastEntryId);

    /**
     * Call this after changing #metadata.
     */
    void updateMetadata();

    std::string path;

    /**
     * Opaque metadata that the log keeps track of.
     */
    RaftLogMetadata::Metadata metadata;

  private:

    std::vector<uint64_t> getEntryIds() const;

    Entry read(const std::string& entryPath) const;


    /** index is EntryId - 1 */
    std::vector<Entry> entries;

    // Log is not copyable
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
};

} // namespace LogCabin::Server::RaftConsensusInternal
} // namespace LogCabin::Server
} // namespace LogCabin

#endif /* LOGCABIN_SERVER_RAFTLOG_H */
