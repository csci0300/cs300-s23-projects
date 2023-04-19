#include "shard.hpp"

bool is_valid(const std::string& s) {
  for (auto&& c : s) {
    if (std::find(VALID_CHARS.cbegin(), VALID_CHARS.cend(), std::toupper(c)) ==
        VALID_CHARS.cend())
      return false;
  }
  return true;
}

std::string bucket_to_str(size_t bucket, size_t granularity) {
  // Get the bucket's indices at each granularity within VALID_CHARS to
  // construct the string
  std::array<size_t, MAX_GRANULARITY> indices{};
  size_t n_left = bucket;

  // Use reverse order through GRANULARITY_OPTS, as we want to find the largest
  // bucket at the current index before moving onto the next one.
  //
  // For example, with granularity = 3, we would iterate from GRANULARITY_OPTS[2
  // -> 0], or { 1296, 36, 1 }. The 1337th bucket would then have indices { 1,
  // 1, 5 }, or "115":
  // - At index 0, we have 1337 / 1296 = 1, with 41 left. This corresponds to
  // the character '1'
  //   within VALID_CHARS.
  // - At index 1, we have 41 / 36 = 1, with 5 left. This corresponds to '1' as
  // well.
  // - At index 2, we have 5 / 1 = 5. This corresponds to '5'.
  for (size_t i = 0; i < granularity; i++) {
    size_t index = n_left / GRANULARITY_OPTS[granularity - i - 1];
    indices[i] = index;
    n_left = n_left % GRANULARITY_OPTS[granularity - i - 1];
  }

  // now, construct res from given indices
  std::string res(granularity, '\0');
  for (size_t i = 0; i < granularity; i++) {
    res[i] = VALID_CHARS[indices[i]];
  }
  return res;
}

size_t str_to_bucket(const std::string& s) {
  // Get the granularity of the string; this should be its size, so "AAB" -> 3.
  size_t granularity = s.size();

  // Find the bucket number at this granularity: for each character c, find its
  // index within VALID_CHARS; this advances the bucket number by the number of
  // buckets at the granularity level.
  //
  // For example, with bound "300", the bucket number within the bucket space is
  // 3888:
  // - At character '3', we're at the highest (i.e. least) granularity level, so
  // each character in VALID_CHARS corresponds to 1296 buckets. '3' is at
  // index 3. This gives bucket += 3 * 1296.
  // - For both characters '0', the index is 0, so at granularity levels 1 and 2
  // (with bucket numbers 36 and 1 respectively) we have bucket += 0 * (36 + 1).
  size_t bucket = 0;
  for (size_t i = 0; i < granularity; i++) {
    // Find the character's index within VALID_CHARS, or throw an error.
    auto pos =
        std::find(VALID_CHARS.cbegin(), VALID_CHARS.cend(), std::toupper(s[i]));
    assert(pos != VALID_CHARS.cend());
    size_t idx = pos - VALID_CHARS.cbegin();
    bucket += idx * GRANULARITY_OPTS[granularity - i - 1];
  }
  return bucket;
}

std::vector<Shard> split_into(size_t n_shards) {
  assert(n_shards > 0);

  // See how many "buckets" we need, and its corresponding granularity level.
  size_t n_buckets = 0, granularity = 0;
  for (int i = 1; i <= MAX_GRANULARITY; i++) {
    if (auto opt = GRANULARITY_OPTS[i]; n_shards <= opt) {
      n_buckets = opt;
      granularity = i;
      break;
    }
  }

  // Get size of each shard
  size_t shard_size = n_buckets / n_shards;
  size_t remainder = n_buckets % n_shards;
  std::vector<size_t> shard_sizes(n_shards, shard_size);
  for (size_t i = 0; i < remainder; i++) {
    shard_sizes[i] += 1;
  }

  // For each shard, get its lower and upper bounds, then convert into string
  // representation
  std::vector<Shard> shards(n_shards);
  size_t lower = 0;
  for (size_t i = 0; i < n_shards; i++) {
    size_t upper = lower + shard_sizes[i] - 1;
    std::string lower_bucket = bucket_to_str(lower, granularity),
                upper_bucket = bucket_to_str(upper, granularity);
    shards[i] = {lower_bucket, upper_bucket};
    lower += shard_sizes[i];
  }
  return shards;
}

// Sort shards in ascending order, by the lower bound.
void sort_shards(std::vector<Shard>& shards) {
  return std::sort(
      shards.begin(), shards.end(),
      [](const Shard& a, const Shard& b) { return a.lower < b.lower; });
}

std::pair<Shard, Shard> split_shard(const Shard& shard) {
  assert(shard.lower != shard.upper);
  size_t lower = str_to_bucket(shard.lower), upper = str_to_bucket(shard.upper);
  uint64_t midpoint = lower + (upper - lower) / 2;
  // get new bounds for each shard (upper bound of lower shard, and lower bound
  // of upper shard)
  std::string lower_upper = bucket_to_str(midpoint, shard.granularity()),
              upper_lower = bucket_to_str(midpoint + 1, shard.granularity());
  return std::pair<Shard, Shard>{{shard.lower, lower_upper},
                                 {upper_lower, shard.upper}};
}

std::pair<Shard, Shard> split_shard(const Shard& shard, uint64_t at) {
  size_t lower = str_to_bucket(shard.lower), upper = str_to_bucket(shard.upper);
  assert(lower < upper);
  // get new bounds for each shard (upper bound of lower shard, and lower bound
  // of upper shard)
  std::string lower_upper = bucket_to_str(at, shard.granularity()),
              upper_lower = bucket_to_str(at + 1, shard.granularity());
  return std::pair<Shard, Shard>{{shard.lower, lower_upper},
                                 {upper_lower, shard.upper}};
}

std::pair<Shard, Shard> split_shard(const Shard& shard, const std::string& at,
                                    bool first) {
  // check that shard is larger than 1, and split location is within range
  assert(shard.lower != shard.upper);
  assert(shard.lower <= at && at <= shard.upper);
  size_t pos = str_to_bucket(at);
  // get new bounds for each shard (upper bound of lower shard, and lower bound
  // of upper shard)
  std::string lower_upper = bucket_to_str(pos - !first, shard.granularity()),
              upper_lower = bucket_to_str(pos + first, shard.granularity());
  return std::pair<Shard, Shard>{{shard.lower, lower_upper},
                                 {upper_lower, shard.upper}};
}

OverlapStatus get_overlap(const Shard& a, const Shard& b) {
  if (a.upper < b.lower || b.upper < a.lower) {
    /**
     * A: [-----]
     * B:         [-----]
     */
    return OverlapStatus::NO_OVERLAP;
  } else if (b.lower <= a.lower && a.upper <= b.upper) {
    /**
     * A:    [----]
     * B:  [--------]
     * Note: This also includes the case where the two shards are equal!
     */
    return OverlapStatus::COMPLETELY_CONTAINED;
  } else if (a.lower < b.lower && a.upper > b.upper) {
    /**
     * A: [-------]
     * B:   [---]
     */
    return OverlapStatus::COMPLETELY_CONTAINS;
  } else if (a.lower >= b.lower && a.upper > b.upper) {
    /**
     * A:    [-----]
     * B: [----]
     */
    return OverlapStatus::OVERLAP_START;
  } else if (a.lower < b.lower && a.upper <= b.upper) {
    /**
     * A: [-------]
     * B:    [------]
     */
    return OverlapStatus::OVERLAP_END;
  } else {
    throw std::runtime_error("bad case in get_overlap\n");
    return OverlapStatus::NO_OVERLAP;
  }
}
